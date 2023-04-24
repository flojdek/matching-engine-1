#!/bin/bash
docker build -t toy_me . && docker run -i --rm toy_me /app/build/match_engine
