# StreamGraphicsGSI-RL

Version: 1.3.1

Orignal coded based on [diogotr7/AuroraGSI-RocketLeague](https://github.com/diogotr7/AuroraGSI-RocketLeague).

Additional ideas and code used as examples pulled from:

- [SOS Overlay System](https://gitlab.com/bakkesplugins/sos/sos-plugin)
- [ReplayStatsBox](https://github.com/CinderBlocc/ReplayStatsBox)

Designed for use with [OSU Buckeye Gaming Collective's Stream Graphics program](https://gitlab.com/osubgc/stream-graphics).

## Setup

Once installed, it's best to create a new line in the `cfg/binds.cfg` file with the following line:

```text
bind R "plugin reload streamgraphicsgsi-rl"
```

This will ensure that you can reload the plugin as needed.  Sometimes the plugin does not start when joining a match, and pressing `R` in this case will reload the plugin and start the integration.

Additionally, the URL where the GSI JSON will be sent to via HTTP POST is configured via the `streamgraphics_gsi_url` variable. This can be set by pressing `F6` and typing `streamgraphics_gsi_url "http://localhost:8080"`, replacing the url with anything you would like the data to be sent to.  This same text above can also be added to the cfg/config.cfg file in the BakkesMod folder where it can be automatically set when Rocket League starts.

## Notes

- If you are not a Spectator in an online game, you will most likely get scrambled boost values for the opposing team.  Only rely on the boost values when a Spectator.

## Example JSON Output

**Note:** Example may not be complete and list all fields.

```json
{
    "game": {
        "status": 1
    },
    "match": {
        "playlist": 37,
        "team": {
            "0": {
                "blue": 0.75,
                "boost": 1.1763970851898193,
                "goals": 2,
                "green": 0.18000000715255737,
                "name": "Blue",
                "red": 0.0
            },
            "1": {
                "blue": 0.0,
                "boost": 1.7351298332214355,
                "goals": 2,
                "green": 0.23999999463558197,
                "name": "Orange",
                "red": 0.699999988079071
            }
        },
        "time": -1
    },
    "player": {
        "assists": 1,
        "boost": 1.0,
        "demos": 0,
        "goals": 0,
        "name": "enzanki_ars",
        "own_goals": 0,
        "saves": 0,
        "score": 260,
        "shots": 14,
        "spec_slot": 6,
        "team": 1
    },
    "provider": {
        "appid": 13377331,
        "name": "rocketleague"
    },
    "spec_players": {
        "1": {
            "assists": 0,
            "boost": 0.6616412997245789,
            "demos": 0,
            "goals": 2,
            "name": "the rial gamer",
            "own_goals": 0,
            "saves": 0,
            "score": 692,
            "shots": 45,
            "spec_slot": 1,
            "team": 0
        },
        "2": {
            "assists": 1,
            "boost": 0.1509445458650589,
            "demos": 0,
            "goals": 0,
            "name": "ElectiveBeast66",
            "own_goals": 0,
            "saves": 0,
            "score": 496,
            "shots": 42,
            "spec_slot": 2,
            "team": 0
        },
        "3": {
            "assists": 1,
            "boost": 0.3638111650943756,
            "demos": 0,
            "goals": 0,
            "name": "Dozy_Youtube",
            "own_goals": 0,
            "saves": 0,
            "score": 688,
            "shots": 51,
            "spec_slot": 3,
            "team": 0
        },
        "4": {
            "assists": 0,
            "boost": 0.0,
            "demos": 0,
            "goals": 0,
            "name": "",
            "own_goals": 0,
            "saves": 0,
            "score": 0,
            "shots": 0,
            "spec_slot": 0,
            "team": -1
        },
        "5": {
            "assists": 0,
            "boost": 0.28846094012260437,
            "demos": 0,
            "goals": 2,
            "name": "hockey-Packman",
            "own_goals": 0,
            "saves": 0,
            "score": 892,
            "shots": 54,
            "spec_slot": 5,
            "team": 1
        },
        "6": {
            "assists": 1,
            "boost": 1.0,
            "demos": 0,
            "goals": 0,
            "name": "enzanki_ars",
            "own_goals": 0,
            "saves": 0,
            "score": 260,
            "shots": 14,
            "spec_slot": 6,
            "team": 1
        },
        "7": {
            "assists": 1,
            "boost": 0.4466688930988312,
            "demos": 0,
            "goals": 0,
            "name": "Cr33p3r_HD_817",
            "own_goals": 0,
            "saves": 0,
            "score": 320,
            "shots": 22,
            "spec_slot": 7,
            "team": 1
        },
        "8": {
            "assists": 0,
            "boost": 0.0,
            "demos": 0,
            "goals": 0,
            "name": "",
            "own_goals": 0,
            "saves": 0,
            "score": 0,
            "shots": 0,
            "spec_slot": 0,
            "team": -1
        }
    }
}
```
