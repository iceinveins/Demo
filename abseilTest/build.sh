docker run \
  -e USER="$(id -u)" \
  -u="$(id -u)" \
  -v /src/workspace:/src/workspace \
  -v /tmp/build_output:/tmp/build_output \
  -w /src/workspace \
  gcr.io/bazel-public/bazel:latest \
  --output_user_root=/tmp/build_output \
  build //absl/...