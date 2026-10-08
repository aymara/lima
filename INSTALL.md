# Installing LIMA

The installation options (Python package, Docker image, building from source)
are documented at <https://aymara.github.io/lima/install/>.

Building from source is described in detail at
<https://aymara.github.io/lima/install/source/>
(source: [`docs/install/source.md`](docs/install/source.md)). In short, under
GNU/Linux, once the dependencies are installed:

```bash
git clone https://github.com/aymara/lima.git
cd lima
git submodule update --init
(cd extern && ./download_libtorch.sh)
source ./setenv-lima.sh -m release
./gbuild.sh -m Release -d ON
lima_models.py -l eng-UD_English-EWT
```
