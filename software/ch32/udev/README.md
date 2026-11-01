# udev rules

To give your linux user access to the wch-linke or esp32-s2 programmer
copy the corresponding rules file into `/etc/udev/rules.d/`

then run:
```
sudo udevadm control --reload
sudo udevadm trigger
```

