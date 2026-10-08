# Installing LIMA

| Method | Platforms | Best for |
| --- | --- | --- |
| [Python package](python.md) | Linux x86_64 | Using LIMA from Python, quick tests |
| [Docker image](docker.md) | Any system running Docker | The latest version without building it |
| [Linux packages](linux-packages.md) | Debian 12, Ubuntu 22.04 | System-wide installation |
| [Windows](windows.md) | Windows 64 bits | Currently unsupported (build broken) |
| [Build from source](source.md) | GNU/Linux | Development, custom builds |

LIMA is known to work under macOS too, but there is no standard build procedure
or binary package for it.

Whatever the method, you then need to
[install language models](../usage/models.md) before analyzing text with the
neural pipelines.
