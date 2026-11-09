#!/bin/sh

SITE=example.com

# You can wait more than 0.5s to be more considerate of bandwith
wget \
     --wait=0.5 \
     --recursive \
     --no-clobber \
     --page-requisites \
     --html-extension \
     --convert-links \
     --restrict-file-names=windows \
     --domains $SITE \
     --no-parent \
         $SITE
