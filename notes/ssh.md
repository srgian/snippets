run command with timeout and pipe output to file on another server (local forward already set up)
```bash
timeout 36000 iostat -d 60 -t mmcblk1p4 mmcblk1p5 | ssh -p 9999 user@localhost -T 'cat - > ~/data.txt'
```
