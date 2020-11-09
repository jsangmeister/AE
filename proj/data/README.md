This folder contains instances for testing purposes. The `scripts` folder contains two different approaches for instance generation:

- `countries` loads data for city labels from simplemaps.com
- `generated` generates random instances

After generation, the script `move_instances.sh` moves all generated instances to the `instances` folder, where only the pure instances are contained.