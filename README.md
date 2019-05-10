# p2p-transfer

## 编译

按常规cmake方式编译即可，只支持ubuntu.

## 运行

### 公网服务器
```
./server
```

### NAT内设备A
```
./client -n client1 -t 4 -h 公网IP -p 7000 
```

### NAT内设备B
```
./client -n client2 -t 4 -h 公网IP -p 7000 -c client1
```

**注意**:
- 同一局域网内的穿透未实现
- 当两台设备的NAT类型均为对称性NAT时，无法穿透
