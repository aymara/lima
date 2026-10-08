# Docker image

Docker images are built by GitHub Actions from the LIMA sources and published
on Docker Hub. This is the easiest way to get the latest version of LIMA.

| Image | Content |
| --- | --- |
| [`aymara/lima-ubuntu22.04`](https://hub.docker.com/r/aymara/lima-ubuntu22.04) | LIMA on Ubuntu 22.04 |
| `ghcr.io/aymara/lima:master` | LIMA on Debian 12 (GitHub Container Registry), latest `master` |

The Docker Hub image is tagged so that you can pin the exact code it
contains:

| Tag | Meaning |
| --- | --- |
| `latest` | Latest build of the `master` branch |
| `YYYYMMDD` | Build of `master` from that day |
| `sha-<commit>` | Build of that commit |
| `<x.y.z>` | Release `vx.y.z` |

## Running LIMA in a container

```bash
docker pull aymara/lima-ubuntu22.04:latest
docker run -it --rm -v "$PWD":/data aymara/lima-ubuntu22.04:latest bash
```

The image does not include language models. Inside the container, install the
ones you need and analyze your files:

```bash
lima_models.py -l eng-UD_English-EWT
analyzeText -l eng-UD_English-EWT -p deepud /data/my-text.txt
```

To keep the models between runs, mount a volume on the models directory, e.g.
`-v lima-models:/root/.local/share/lima`.

## Running the LIMA server

The image also contains `limaserver`, an HTTP server exposing the analyzer
(port 8080 by default). Give it the languages and pipelines to load (defaults
are read from `lima-server.xml`; see `limaserver --help`):

```bash
docker run -p 8080:8080 -v lima-models:/root/.local/share/lima \
    aymara/lima-ubuntu22.04:latest \
    limaserver --language eng-UD_English-EWT --pipelines deepud
```

Then send it some text:

```bash
curl "http://localhost:8080/?lang=eng-UD_English-EWT&pipeline=deepud" \
    --data-binary @file.txt
```

## Custom configuration

To change LIMA's configuration inside the container, put your own
configuration files in a folder of the host, mount it in the container and make
the `LIMA_CONF` variable point to it, before the system configuration (see
[Configuring LIMA](../usage/configuration.md)):

```bash
docker run -it --rm -v "$PWD/my-conf":/my-conf \
    -e LIMA_CONF=/my-conf:/usr/share/config/lima \
    aymara/lima-ubuntu22.04:latest bash
```
