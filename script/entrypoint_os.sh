#!/usr/bin/env bash

CONFIG_FILE="/market-simulator/quod/data/cfg/market_simulator.xml"
TEMPLATE_DIR="/template"
DATA_DIR="/market-simulator/quod/data"

INSTANCE_ID=${INSTANCE_ID:-"SIMULATOR"}
PREFIX=${PREFIX:-"QUOD"}

if [ ! -d "$DATA_DIR/cfg" ]; then
    echo "Directory $DATA_DIR/cfg not found. Copying entire cfg/ folder from templates."
    cp -r "$TEMPLATE_DIR/cfg" "$DATA_DIR/" || { echo 'Failed to copy cfg directory'; exit 1; }
else
    echo "Directory $DATA_DIR/cfg exists. Checking individual files/subdirectories inside for defaults."
    if [ ! -f "$DATA_DIR/cfg/configSim.txt" ]; then
        echo "Copying missing configSim.txt"
        cp "$TEMPLATE_DIR/cfg/configSim.txt" "$DATA_DIR/cfg/" || { echo 'Failed to copy configSim.txt'; exit 1; }
    fi

    if [ ! -f "$DATA_DIR/cfg/market_simulator.xml" ]; then
        echo "Copying missing market_simulator.xml"
        cp "$TEMPLATE_DIR/cfg/market_simulator.xml" "$DATA_DIR/cfg/" || { echo 'Failed to copy market_simulator.xml'; exit 1; }
    fi

    if [ ! -d "$DATA_DIR/cfg/default" ]; then
        echo "  Directory $DATA_DIR/cfg/default not found. Copying default/ folder from templates."
        cp -r "$TEMPLATE_DIR/cfg/default" "$DATA_DIR/cfg/" || { echo 'Failed to copy default directory'; exit 1; }
    else
        echo "  Directory $DATA_DIR/cfg/default exists. Checking individual files inside."
        if [ ! -f "$DATA_DIR/cfg/default/FIX50SP2_marketsimulator.xml" ]; then
            echo "Copying missing FIX50SP2_marketsimulator.xml"
            cp "$TEMPLATE_DIR/cfg/default/FIX50SP2_marketsimulator.xml" "$DATA_DIR/cfg/default/" || { echo 'Failed to copy FIX50SP2_marketsimulator.xml'; exit 1; }
        fi
        if [ ! -f "$DATA_DIR/cfg/default/FIXT11_marketsimulator.xml" ]; then
            echo "Copying missing FIXT11_marketsimulator.xml"
            cp "$TEMPLATE_DIR/cfg/default/FIXT11_marketsimulator.xml" "$DATA_DIR/cfg/default/" || { echo 'Failed to copy FIXT11_marketsimulator.xml'; exit 1; }
        fi
    fi
fi

if [ -f "$CONFIG_FILE" ]; then
  echo "Using configuration file: $CONFIG_FILE"
  exec ./bin/marketsimulator --id $INSTANCE_ID --pf $PREFIX -f $CONFIG_FILE
else
  echo "Error: Configuration file not found: $CONFIG_FILE"
  exit 1
fi