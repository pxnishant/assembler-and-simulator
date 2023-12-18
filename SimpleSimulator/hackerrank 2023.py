N = input()
flag = 0
for i in range(N):
    a = input()
    if (a<1000 and a%2==0):
        print(a, end=" ")
        flag = 1
        
if (flag == 0):
    print(-1)
        