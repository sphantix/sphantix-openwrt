#!/bin/bash
CURR_DIR=${PWD}

#########################################################
#       Install customer feeds
#########################################################
cp ../feeds.conf.default ../feeds.conf
echo "src-link luci $CURR_DIR/packages/luci" >> ../feeds.conf
echo "src-link customer $CURR_DIR/packages/others" >> ../feeds.conf
cd ../
./scripts/feeds update luci
./scripts/feeds update customer
./scripts/feeds install -a -p luci
./scripts/feeds install -a -p customer


echo "*****************************************************************************************"
echo "*****************************************************************************************"
echo "********                                                                        *********"
echo "********  Success! -- Already install all customer packages, please run make!   *********"
echo "********                                                                        *********"
echo "*****************************************************************************************"
echo "*****************************************************************************************"
