#!/bin/bash
#
#  COPYRIGHT NOTICE
#  Copyright (C) 2016 Baidu Corporation, Inc. All rights reserved
#
#  Author             :zhangjiajie@baidu.com
#  File Name          :install.sh
#  Create Date        :2020/03/19 17:03
#  Last Modified      :2020/03/19 17:03
#  Description        :NA
#

rm ~/.vimrc -rf
rm ~/.vim -rf

ln -s $(pwd)/.vimrc ~/.vimrc
ln -s $(pwd)/.vim ~/.vim

