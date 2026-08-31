# Idea
I wish to create a mini project. This project will be handcoded. 
The idea is to have a smartTV (with modding possiblities). 

It _should_ be able to display avalaible movies/tv shows from a choosen list of streaming services (ie the ones you are sub'd to).
If a movie/tv show is selected, it should based on a list of priorities display the movie for the user on the choosen website (**_seemlessly_**)

Furthermore I wish to integrate emulation of simple games - PS, DS and so on.

Lastly, I hope to add seemless VPN integregration (first using NordVPN).
This should, in theory, allow the UI to display movies from around the world. Then if a movie is choosen, the application will utilize the VPN to change the location, and thus display the movie from said location.
This will NOT be an intented usage, but rather a proof of concept. 

Naturally the most important feature here is the responsiveness when using the remote. 

# Hardware
This is to be used on a lenovo thinkstation m910x tiny, running some low impact linux distro (_not sure yet_)

# Overall Archetecture idea
The app should be seperated as much as possible, to ensure clean integration or usage.
Thus, it will in _theory_ consists of three seperate applications:

Core owns the meaning of the application. UI only presents it. Sync only keeps its data fresh.

## Core
This is the heart of the operation. It will handle both querying the SQL server, along with opening various websites. 

## Sync
This one, will periodicly sync the sql database. What I mean by this is:
It will lookup data from various websites, normalize it and update the sql database with.

This means if a movie is removed from Netflix, but added to HBO, this should be reflacted in the databse (and so also the UI)

## UI
This one is self explanatory; it is the UI. It should interact only with the core using interfaces.
This should allow for easy additions, along with _mods_.

# Intented Structure
                 |--------------------------|
                 |        smarttv-ui        |
                 |                          |
                 |       Qt / QML           |
                 |------------|-------------|
                              |
                           Core API
                              |
                 |------------|-------------|
                 |       smarttv-core       |
                 |                          |
                 | Catalog                  |
                 | Search                   |
                 | Provider selection       |
                 | Playback                 |
                 | VPN                      |
                 | Games                    |
                 | Settings                 |
                 | Plugin manager           |
                 |-------|----------|-------|
                         |          |
                         |          |
                     SQLite     External apps
                         |       / emulators
                         |
                 |------------------|
                 |   smarttv-sync   |
                 |                  |
                 | Fetch            |
                 | Normalize        |
                 | Deduplicate      |
                 | Update           |
                 |------------------|
