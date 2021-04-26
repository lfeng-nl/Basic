![netfilter](./image/netfilter.png)

![iptables - Wikipedia](./image/Netfilter-packet-flow.png)

检查点: 也被称为链 (Chain), 例如`INPUT, PREROUTING, OUTPUT`

iptables 表的作用，就是在某个具体的“检查点”(比如 Output)上，按顺序执行几 个不同的检查动作(比如，先执行 nat，再执行 filter)

