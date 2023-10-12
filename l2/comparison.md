# Comparison of effectiveness

## 3 million coordinates (1 million of three points)
### Python
```
1  thread:   ~2s
```

### C:
```
1    thread:  ~0.0225s

2    threads: ~0.0175s

5    threads: ~0.0010s

10   threads: ~0.0075s

20   threads: ~0.0045s

50   threads: ~0.0034s

100  threads: ~0.0035s

500  threads: ~0.0075s

750  threads: ~0.0110s

1000 threads: ~0.0155s
```