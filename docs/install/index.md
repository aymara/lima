# Installing LIMA

--8<-- "versions.md"

| Method | Platforms | Best for |
| --- | --- | --- |
| [Python package](python.md) | Linux x86_64, Windows x64, macOS (Apple Silicon) | Using LIMA from Python, quick tests |
| [Docker image](docker.md) | Any system running Docker | The latest version without building it |
| [Linux packages](linux-packages.md) | Debian 12, Ubuntu 22.04 | System-wide installation |
| [Windows](windows.md) | Windows 10/11 x64 | Native build with Visual Studio 2022 |
| [Build from source](source.md) | GNU/Linux | Development, custom builds |

Under macOS (Apple Silicon), the Python package is the supported way to use
LIMA; there is no documented native build procedure yet.

Whatever the method, you then need to
[install language models](../usage/models.md) before analyzing text with the
neural pipelines.
