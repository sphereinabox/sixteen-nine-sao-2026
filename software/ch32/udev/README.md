# udev rules

To give your linux user access to the wch-linke or esp32-s2 programmer
copy the corresponding rules file into `/etc/udev/rules.d/` then reload udev rules:

for example:
```
sudo cp 55-wch-linke.rules /etc/udev/rules.d/
sudo chown root:root /etc/udev/rules.d/*.rules
sudo udevadm control --reload
sudo udevadm trigger
```

