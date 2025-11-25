# BINO

Bino is an experimental project designed to explore both classical and modern techniques for building virtual-reality systems. It combines CUDA-accelerated machine-learning models with a C++ real-time engine to investigate end-to-end VR pipelines, from low-latency interfaces to on-device inference and rendering (long-term goal).

## Quick Start

Build and run in Docker:

```bash
docker compose -f docker/docker-compose.yaml up
```

### Docker GTK Fix

If you see GTK initialization errors, run this on your host:

```bash
xhost +local:docker
```
