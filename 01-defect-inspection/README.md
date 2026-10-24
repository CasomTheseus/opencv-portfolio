# 01 · 工业缺陷检测

用“首件模板 + 差分”找零件阵列中的表面缺陷。输入一张工件图，输出每个零件的 OK/NG、差异像素数和标注图。

![Defect inspection](../docs/images/defect-inspection.jpg)

## 数据

- 图像：`assets/defect_parts.png`（1280×720，程序合成）
- 真值：`assets/defect_parts_ground_truth.csv`
- 8 个零件，其中 3 个带缺陷：
  - #3 右上角缺口（chip）
  - #6 表面划痕（scratch）
  - #8 缺失螺栓孔（missing hole）

合成数据的好处是缺陷位置、类型和数量都已知，方便算漏检/误检；同样的流程可以迁移到真实工件图，只需重新标定阈值。

## 处理流程

1. **灰度 + OTSU 二值化**：自动判断极性（亮零件/暗背景 或 反过来），避免写死 `THRESH_BINARY_INV`。
2. **开运算**：去掉小噪点，分离相邻零件。
3. **外轮廓 + 过滤**：按面积、宽高、高度占比筛掉背景和碎片。
4. **按行排序**：先按 y 分行，再在行内按 x 排序，得到稳定编号（1…8）。
5. **首件做模板**：取排序后的第一个零件作为参考；也可以用 `--template` 指定。
6. **逐件差分**：
   - 裁剪 ROI，缩放到模板尺寸；
   - `absdiff` 同时捕捉“缺料”和“多料”；
   - 清掉 2 px 重采样边界，再做 3×3 开运算；
   - 统计非零像素和缺陷连通域数量。
7. **判定**：`diff_pixels > --defect-pixels` 判为 NG。

## 运行

```bash
./build/bin/defect_inspection \
    --input assets/defect_parts.png \
    --output outputs/defect
```

带窗口预览：

```bash
./build/bin/defect_inspection \
    --input assets/defect_parts.png \
    --output outputs/defect --gui
```

### 常用参数

| 参数 | 默认值 | 说明 |
|---|---|---|
| `--input` | 必填 | 图片或目录 |
| `--output` | `outputs/defect` | 输出目录 |
| `--template` | 自动 | 使用固定模板，而不是首件 |
| `--min-area` | 500 | 最小轮廓面积 |
| `--defect-pixels` | 40 | 判定 NG 的差异像素阈值 |
| `--open-kernel` | 3 | 二值化后的开运算核 |
| `--invert` | 自动 | 强制阈值极性：0 正常 / 1 反相 |
| `--limit` | 0 | 最多处理多少张图 |
| `--gui` | 关 | 显示标注结果 |

## 输出

```
outputs/defect/
├── defect_parts_annotated.jpg
└── parts.csv
```

`parts.csv` 字段：

```
image,part,x,y,width,height,diff_pixels,blobs,verdict,elapsed_ms
```

## 结果

| 零件 | 真值 | diff_pixels | blobs | 判定 |
|---|---|---|---|---|
| #1 | OK | 0 | 0 | OK |
| #2 | OK | 0 | 0 | OK |
| #3 | chip | 1120 | 1 | NG |
| #4 | OK | 0 | 0 | OK |
| #5 | OK | 0 | 0 | OK |
| #6 | scratch | 899 | 2 | NG |
| #7 | OK | 0 | 0 | OK |
| #8 | missing_hole | 281 | 1 | NG |

- 3 个缺陷全部检出，4 个正常件零误检。
- 正常件的 diff 恰好是 0，因为合成零件同尺寸、同纹理、同光照；
  真实产线会有几个像素的抖动，阈值需要按产品调。
- 单张耗时约 **0.85 ms**（Apple Silicon / Release）。

## 设计中的几个细节

- **极性自动判断**：同时尝试 `BINARY` 和 `BINARY_INV`，谁找到的候选零件多就用谁。
- **稳定排序**：直接按 `y` 排序在零件轻微错位时会乱序；这里先用中位高度的一半做行容差分组，再行内排序。
- **边界清零**：ROI 缩放到模板尺寸后，边缘会有重采样噪声；把 2 px 边界清零再做开运算，可以把正常件的 diff 压到 0。
- **`absdiff` 而不是 `subtract`**：`subtract(tpl, roi)` 会把“ROI 比模板亮”的区域截断成 0，漏掉多料/凸起类缺陷。

## 局限

- 依赖“同姿态、同尺度、同光照”的首件模板；零件旋转或镜头变化大时需要先做配准/对齐。
- 阈值是演示数据上标定的；真实场景要做光照归一化和阈值标定。
- 只做 2D 表面差异，不能判断深度方向的缺陷（需要结构光/线激光）。

## 对应课程

原始版本是 [archive/labs/3.IndustryKnifeDetection/IndustryKnifeDetection.cpp](../archive/labs/3.IndustryKnifeDetection/IndustryKnifeDetection.cpp)。
原图 `assets/defect_blades.jpg` 是一块连续工件（整块金属 + 槽位），不适合“按单件模板比对”，所以重构时改用可控的合成零件阵列，原图保留作对照。
