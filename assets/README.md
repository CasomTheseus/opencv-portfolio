# 演示数据

本目录的演示数据分两类：

## 程序合成（推荐，可完全复现）

由 [`tools/make_demo_data.cpp`](../tools/make_demo_data.cpp) 生成：

| 文件 | 说明 |
|---|---|
| `defect_parts.png` | 1280×720，8 个零件，3 个缺陷（chip / scratch / missing hole） |
| `defect_parts_ground_truth.csv` | 上述零件的真值 |
| `label_template.png` | 720×440 的合成标签模板 |
| `labels/label_ok_01.jpg` | 带标签的场景（轻微透视） |
| `labels/label_ok_02.jpg` | 带标签的场景（大角度透视） |
| `labels/label_missing.jpg` | 无标签的干扰场景 |
| `generated/demo_video.mp4` | 640×480 / 30 fps / 240 帧的合成运动视频（`.gitignore` 忽略，运行时生成） |

重新生成：

```bash
./build/bin/make_demo_data --what all --out assets
```

## 课程素材（仅作对照）

| 文件 | 说明 |
|---|---|
| `defect_blades.jpg` | 课程练习用的连续工件照片，保留用于说明“为什么合成数据更适合做模板比对”。默认演示不使用。 |

如果你要替换成自己的产线图，直接把 `--input` 指过去即可；缺陷检测会以目录中的第一张图/第一个零件建立模板。

## 隐私说明

- 合成标签和视频不含任何真实个人信息。
- 原始标签案例中的真实卡片照片包含个人金融信息，未纳入本仓库。
- 如果 `defect_blades.jpg` 涉及版权问题，删除该文件即可，不影响构建和演示。
