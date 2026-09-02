#!/bin/bash

set -e

if [ ! -f .env ]; then
    echo "ERROR: .env file not found"
    exit 1
fi

set -a
source .env
set +a

if [ -z "$REALDEBRID_API_KEY" ]; then
    echo "ERROR: REALDEBRID_API_KEY is not set"
    exit 1
fi

if [ -z "$MOVIEOFTHENIGHT_API_KEY" ]; then
    echo "ERROR: MOVIEOFTHENIGHT_API_KEY is not set"
    exit 1
fi

cmake --build cmake-build-debug --target smarttv-core

rm -f /tmp/smartTV-core

./cmake-build-debug/apps/core/smarttv-core