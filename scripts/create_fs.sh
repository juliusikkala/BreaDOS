#! /bin/bash

OUTPUT_DIR=$1
KERNEL=$2
GRUB_CFG=$3

mkdir -p $OUTPUT_DIR
mkdir -p $OUTPUT_DIR/boot
mkdir -p $OUTPUT_DIR/grub

cp $KERNEL $OUTPUT_DIR/boot
cp $GRUB_CFG $OUTPUT_DIR/grub
