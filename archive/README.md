# 原始课程练习（归档）

这里保留学习过程中的原始代码，方便回看“当时是怎么写的”。

- **仅源码**：41 个文件（5 个 quick-tutorial 文件 + 35 个练习 `.cpp` + 1 个头文件）。
- **不参与构建**：根目录的 CMake 不会编译这里。
- **路径已脱敏**：所有 `/Users/<name>` 已替换为 `~`。
- **模型未入库**：`opencv_face_detector_uint8.pb`（2.7 MB）、`mobilenet_iter_73000.caffemodel`（23 MB）没有纳入，避免仓库膨胀和许可问题。
- **入口约定**：原工程所有练习共用一个 target，每个文件整体包在 `#ifdef RUN_MAIN` 里；同一时间只能取消一个文件的注释。

## 目录

```
archive/
├── quick-tutorial/              # 29 节基础课
│   ├── quick_opencv.h
│   ├── quick_demo.cpp
│   ├── main.cpp
│   ├── mouse_zoom_demo.cpp
│   └── face_detect_demo/main.cpp
└── labs/
    ├── 1/                       # 轮廓、形态学、视频、光流
    ├── 2/                       # 特征、DNN、ORBDetector
    ├── 3.IndustryKnifeDetection/
    └── 4.自定义对象检测/
```

## 映射到新项目

| 原始文件 | 新位置 |
|---|---|
| `labs/3.IndustryKnifeDetection/IndustryKnifeDetection.cpp` | [01-defect-inspection](../01-defect-inspection/) |
| `labs/2/FeatureExtraction/ORBDetector.cpp` + `头文件/ORBDetector.h` | [02-label-localization](../02-label-localization/) |
| `labs/1/039`、`labs/1/040–042`、`labs/1/038` | [03-video-analytics](../03-video-analytics/) |
| `quick-tutorial/quick_demo.cpp` | 基础 API 参考 |
| `labs/2/DNN_OBJECT_DETECTION/` | 归档，未纳入本次重构 |

原始代码里已知的问题（trackbar 窗口名、直方图通道复用、HOG 滑窗特征等）记录在
[docs/learning-log.md](../docs/learning-log.md) 的“修掉的问题”一节。
