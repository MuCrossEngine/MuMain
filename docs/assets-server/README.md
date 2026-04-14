# Assets Server Manifest Guide

This folder contains references used to publish Android game assets.

## Files

- `Data/assets-manifest.txt`: production template/example for manifest format.

## Generate a real production manifest

From repository root:

```bash
tools/generate_assets_manifest.sh \
  --asset-root /path/to/asset-server-root \
  --output /path/to/asset-server-root/Data/assets-manifest.txt \
  --zip-extract Data
```

Notes:

- `--asset-root` accepts either the server root (that contains `Data/`) or the `Data/` folder itself.
- All files under `Data/` are included except `assets-manifest.txt`, `*.crc32`, `*.extracted`.
- `.zip` files are automatically marked as package entries (`archive=1|extract=<zip-extract>`).
