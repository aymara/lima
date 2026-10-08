!!! warning "The PyPI release lags behind the source code"
    The current PyPI release, `aymara` 0.5.0b6 (April 2024), was built before
    LIMA's neural modules moved from TensorFlow to libtorch. It ships its own
    model installers (`lima_models.py` for the legacy models and
    `deeplima_models` for the libtorch ones) and its own `ud-eng`/`ud-fra`
    pipelines. A new release built from the current code is in preparation;
    until then, the [Docker image](../install/docker.md) or a
    [source build](../install/source.md) give you the latest version.
