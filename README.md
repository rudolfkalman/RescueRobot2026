# レスコン2026 開発リポジトリ

このリポジトリは、**レスキューロボットコンテスト**に向けた制御システムおよびシミュレーション環境を管理します。

---

## 🛠 開発環境

> 🚨
> **Docker / Docker Compose 必須。**
> コンテナ外（ホスト側）でのビルドや作業は、環境汚染の原因になるため**禁止**します。必ずコンテナ内で作業してください。\
> ※ ただし、**コードの編集はホスト側のVS Codeなどのエディタで構いません。**（コンテナとファイルが同期されています）

### Windowsユーザーへの注意
Windowsで開発を行う場合は、必ず **WSL2** 上でコンテナを起動してください。
また、実機通信を行う際に **Docker Desktop + Windowsファイアウォール** の影響でROS 2の通信が遮断されるケースがあります。
通信が通らない場合は、必ず **`docs/document4windows.pdf`** の手順に従って設定を行ってください。

### 起動手順
作業開始時は、リポジトリのルートで必ず以下を実行すること。

```bash
# 全てのコンテナを起動し、開発環境をセットアップします
./scripts/start_all.sh
```

---

## ⚠️ 開発の掟（重要！）

円滑なチーム開発（および管理者の精神衛生）のため、以下のルールを**絶対遵守**すること。

### 1. ブランチ運用

- `master` ブランチへの直接 push は**禁止**（物理ロック済み）
- `develop` ブランチも直接触らないこと
- 作業は必ず `develop` からブランチを切って行う
- 命名規則： `feature/名前-作業内容`（例：`feature/tanaka-urdf-fix`）
- 作業完了後、`develop` ブランチに対して **Pull Request (PR)** を作成すること

### 2. コンフリクトの解消は「作業者」の義務

> PRを出す前に、必ず最新の `develop` を自分のブランチに取り込み（merge または rebase）、コンフリクトがないことを確認すること。
> **管理者はコンフリクトの解消作業を代行しません。**

### 3. パッケージ命名・役割分担

`src/` 内のパッケージは以下の Prefix（接頭辞） ルールに従って作成・管理すること。**自分の担当外の Prefix を持つパッケージは編集禁止。**
| Prefix | 担当 | 内容（具体例）|
|---|---|---|
| sim_* | シミュレーション担当| `sim_gazebo`, `sim_description`, `sim_base_drive` 等|
| ctrl_* | 制御コア担当| `ctrl_driver`, `ctrl_logic`, `ctrl_esp32_interface` 等|
| ui_* | UI/通信担当 | `ui_dashboard`, `ui_teleop`, `ui_camera_view` 等|
| ext_* | 外部パッケージ| 外部から取得した既存パッケージ、配布物等|

### 4. 自動検閲 (GitHub Actions)

- PR作成後、自動でビルドチェック（Actions）が走ります
- ❌ がついているPRは、**内容を一切見ません**
- 全て ✅ になるまで自力で修正すること

---

## 🚀 迷った時の3行コマンド集

「作業を始めてから、GitHubに上げるまで」のテンプレです。

```bash
# 1. 最新の状態をもらってブランチを作る
git checkout develop && git pull origin develop
git checkout -b feature/your-name-task

# 2. （作業後）変更を記録してサーバーに送る
git add .
git commit -m "feat: 〇〇のプラグイン設定を追加"
git push origin feature/your-name-task

# 3. GitHubでPRボタンを押す（あとは祈る）
```
