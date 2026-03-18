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

PR作成後、自動で以下のチェックが走ります。

| ジョブ | 内容 |
|---|---|
| `build` | colcon ビルド確認 |
| `cpplint` | Google スタイル準拠チェック |
| `clang-tidy` | 静的解析・命名規則チェック |

- ❌ がついているPRは、**内容を一切見ません**
- 全て ✅ になるまで自力で修正すること
- ローカルで事前確認する場合:
  ```bash
  # cpplint
  find src -name "*.cpp" -o -name "*.hpp" | grep -v "src/ext_" | xargs cpplint

  # clang-tidy（colcon build 後に compile_commands.json が生成される）
  clang-tidy src/ctrl_*/src/*.cpp -p build
  ```

- PR作成後、自動で以下のチェックが走ります

| ジョブ | 内容 |
|---|---|
| `build` | colcon ビルド確認 |
| `cpplint` | Google スタイル準拠チェック |
| `clang-tidy` | 静的解析・命名規則チェック |

- ❌ がついているPRは、**内容を一切見ません**
- 全て ✅ になるまで自力で修正すること
- ローカルで事前確認する場合:
  ```bash
  # cpplint
  find src -name "*.cpp" -o -name "*.hpp" | grep -v "src/ext_" | xargs cpplint

  # clang-tidy（compile_commands.json が必要）
  clang-tidy src/ctrl_*/src/*.cpp -p build
  ```

---

## 📝 命名規則・コーディング規約

### ファイル名・ディレクトリ名

すべて `snake_case` に統一。スペース・大文字・ハイフンは**使用禁止**。テストファイルは `test_` プレフィックスを付ける（例: `test_motor_controller.cpp`）。

### C++（[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) 準拠）

| 対象 | 規則 | 例 |
|---|---|---|
| 変数（ローカル） | `snake_case` | `motor_speed` |
| 変数（メンバ） | `snake_case_` | `node_id_` |
| 関数・クラス・構造体 | `UpperCamelCase` | `GetSensorData()`, `MotorController` |
| 定数・constexpr | `kUpperCamelCase` | `kMaxSpeed` |
| 名前空間 | `snake_case` | `ctrl_driver` |
| マクロ | `UPPER_SNAKE_CASE` | `MAX_RETRY_COUNT` |

インデント: **スペース2つ**、1行: **最大100文字**、`*`/`&` は型側（`int* ptr`）

### Python（[PEP 8](https://peps.python.org/pep-0008/) 準拠）

| 対象 | 規則 | 例 |
|---|---|---|
| 変数・関数 | `snake_case` | `get_sensor_data()` |
| クラス | `UpperCamelCase` | `MotorController` |
| 定数 | `UPPER_SNAKE_CASE` | `MAX_SPEED` |
| プライベートメンバ | `_snake_case` | `self._node_id` |

インデント: **スペース4つ**、1行: **最大100文字**、型アノテーション・docstring を使用すること。

### 共通

- コメントは日本語・英語どちらか一方に**1ファイル内で統一**（混在禁止）
- マジックナンバー禁止（定数または設定ファイルに切り出す）
- `TODO` / `FIXME` には担当者名と日付を記載: `// TODO(tanaka 2026-01-01): 内容`
- 不要なコメントアウトコードはコミット前に削除すること

---

### 🚫 キャッシュ・不要ファイルのアップロード禁止

以下のファイル・ディレクトリは **`.gitignore` で管理されているが、誤ってコミットしないよう各自も注意**すること。  
**発見した場合は即座に担当者に報告し、履歴から削除する。**

| 禁止対象 | 具体例 |
|---|---|
| ビルド成果物 | `build/`, `install/`, `log/`, `*.o`, `*.so`, `*.pyc` |
| Python キャッシュ | `__pycache__/`, `*.pyo`, `.pytest_cache/` |
| C++ キャッシュ | `.cache/`, `compile_commands.json`（自動生成分）|
| エディタ設定 | `.vscode/settings.json`（個人設定）, `.idea/` |
| OS 生成ファイル | `.DS_Store`, `Thumbs.db` |
| 認証情報・秘密鍵 | `*.pem`, `*.key`, `.env`, `credentials.json` |
| ログファイル | `*.log`, `ros2_logs/` |

> 💡 **誤ってコミットしてしまった場合**は `git rm --cached <file>` で追跡を解除し、`.gitignore` に追記の上、PRで対応すること。  
> 認証情報が含まれた場合は**必ず管理者に即時報告**すること。

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