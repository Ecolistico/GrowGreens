#!/bin/bash
sudo cp wpa_supplicant.conf.orig /etc/wpa_supplicant/wpa_supplicant.conf.orig
sudo cp dhcpcd.conf.ap /etc/dhcpcd.conf.ap
sudo cp dhcpcd.conf.orig /etc/dhcpcd.conf.orig
sudo cp hostapd.ap /etc/default/hostapd.ap
sudo cp hostapd.conf.orig /etc/hostapd/hostapd.conf.orig
sudo cp hostapd1.ap /etc/init.d/hostapd.ap
sudo cp hostapd1.orig /etc/init.d/hostapd.orig
sudo cp dnsmasq.conf.ap /etc/dnsmasq.conf.ap
sudo cp dnsmasq.conf.orig /etc/dnsmasq.conf.orig
sudo cp sysctl.conf.ap /etc/sysctl.conf.ap
sudo cp sysctl.conf.orig /etc/sysctl.conf.orig
sudo cp rc.local.ap /etc/rc.local.ap
sudo cp rc.local.orig /etc/rc.local.orig
$SHELL