# 02 · 标签定位与有无判定

给定一张标签模板，在场景图里找到它、估计单应性、输出矫正图，并判断标签是否存在。

![Label localisation](../docs/images/label-localization.jpg)

## 问题

工业现场经常需要确认“标签贴没贴、贴歪没贴歪”。与其训练一个检测器，不如用模板匹配：
标签本身有大量稳定角点，ORB + RANSAC 单应性可以用很少的样本完成定位，还能顺便把透视变形矫正回来，交给后续 OCR/解码。

## 流程

1. **模板特征**：灰度化，`ORB::create(2000)` 提取关键点和描述子。
2. **场景特征**：同样提取 ORB。
3. **暴力匹配 + Lowe ratio test**：`knnMatch(k=2)`，保留 `d0 < 0.75 * d1` 的匹配。
4. **RANSAC 单应性**：`findHomography(scene → template, RANSAC, 4px)`。
5. **置信度**：
   - inliers 数量；
   - inlier ratio = inliers / good matches；
   - 平均重投影误差；
   - 反投影四边形面积占比（排除退化/离谱的单应性）。
6. **判定**：`inliers ≥ 12 && inlier_ratio ≥ 0.35 && 四边形面积合理` → OK。
7. **矫正**：`warpPerspective` 把场景映射到模板尺寸，得到摆正的标签图。

## 运行

```bash
./build/bin/label_localization \
    --template assets/label_template.png \
    --input assets/labels \
    --output outputs/labels
```

单张图片也可以：

```bash
./build/bin/label_localization \
    --template assets/label_template.png \
    --input assets/labels/label_ok_01.jpg \
    --output outputs/labels-one
```

### 常用参数

| 参数 | 默认值 | 说明 |
|---|---|---|
| `--template` | 必填 | 模板标签图 |
| `--input` | 必填 | 场景图或目录 |
| `--output` | `outputs/labels` | 输出目录 |
| `--min-inliers` | 12 | 最小 RANSAC 内点数 |
| `--min-score` | 0.35 | 最小内点比例 |
| `--ratio` | 0.75 | Lowe ratio 阈值 |
| `--ransac` | 4.0 | RANSAC 重投影阈值（像素） |
| `--max-features` | 2000 | ORB 特征预算 |
| `--gui` | 关 | 显示标注结果 |

## 输出

```
outputs/labels/
├── label_ok_01_annotated.jpg
├── label_ok_01_aligned.png
├── label_ok_02_annotated.jpg
├── label_ok_02_aligned.png
├── label_missing_annotated.jpg
└── labels.csv
```

`labels.csv` 字段：

```
image,verdict,good_matches,inliers,inlier_ratio,reproj_error,quad_area_ratio,elapsed_ms
```

## 结果

| 场景 | 真值 | good matches | inliers | ratio | 重投影误差 | 判定 |
|---|---|---|---|---|---|---|
| `label_ok_01.jpg` | 有标签 | 521 | 480 | 0.921 | 1.11 px | OK |
| `label_ok_02.jpg` | 有标签（大角度透视） | 386 | 356 | 0.922 | 1.30 px | OK |
| `label_missing.jpg` | 无标签 | 12 | 2 | 0.167 | 0.58 px | NG |

- 两张正常场景的 inlier ratio 都在 0.92 左右，重投影误差约 1 px。
- 缺失场景只剩 12 个偶然匹配、2 个内点，面积校验也为 0，稳定判 NG。
- 平均耗时约 **18 ms/图**（Apple Silicon / Release，1280×960）。

矫正结果：

![Aligned label](../docs/images/label-aligned.png)

## 工程上的几个决定

- **不把 `matches[0]` 当结论**：原练习代码在匹配为空时会越界，且只用第一个匹配的距离判断；这里改成 inliers + ratio + 几何校验三重条件。
- **几何合理性校验**：只有 inliers 多还不够，反投影四边形必须落在画面内、面积占比在 1%–95% 之间，避免背景纹理拼出离谱的单应性。
- **输出矫正图**：定位不是终点，把标签摆正后交给 OCR/条码解码才是完整链路。
- **合成标签**：模板和场景都由 `tools/make_demo_data` 生成，包含二维码块、条码、文字和定位角，专门为 ORB 提供高频角点。

## 局限

- ORB 对弱纹理、重复纹理、强反光和大面积遮挡敏感；这类场景建议换 SIFT/AKAZE 或直接上深度学习。
- 尺度变化过大时需要图像金字塔或多尺度模板。
- 当前只做“有没有 + 在哪 + 摆正”，没有做字符识别和解码；后续可以接 Tesseract/PaddleOCR 或 `cv::QRCodeDetector`。
- 阈值（12 inliers / 0.35 ratio）是针对演示数据调的，实际项目要用正负样本重新标定。

## 对应课程

原练习是 [archive/labs/2/FeatureExtraction/ORBDetector.cpp](../archive/labs/2/FeatureExtraction/ORBDetector.cpp)。
重构时修掉了原 `ORBDetector` 的三个问题：
1. 构造函数里 `orb.release()` 把 ORB 指针置空，导致 `initORB` 段错误；
2. 析构函数只声明没有实现，正常使用会链接失败；
3. `matches[0]` 之前没有判空。

原始标签案例使用了含个人金融信息的真实卡片照片，未纳入仓库；演示数据全部替换为合成标签。
