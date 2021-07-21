rm deeplima.tar
docker build -t deeplima -f build_scripts/Dockerfile . && docker save -o deeplima.tar deeplima

