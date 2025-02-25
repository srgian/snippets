instead of using ~/.ssh/config, for a one time job you can use custom ssh options for git commands
```bash
GIT_SSH_COMMAND="ssh -i ~/.ssh/some_other_key" git push origin main
```