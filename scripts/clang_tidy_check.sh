# ビルド + compile_commands.json 生成
colcon build --symlink-install --cmake-args -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# clang-tidy
find src -name "*.cpp" | grep -v "src/ext_" | while read file; do
  pkg=$(echo "$file" | cut -d/ -f2)
  clang-tidy "$file" -p "build/$pkg" --config-file=.clang-tidy
done