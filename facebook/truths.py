with open('./0.circles') as f:
    lines=f.readlines()
    circles=[]
    for line in lines:
        circle=line.strip().split()
        circle=set(circle[1:])

        circles.append(circle)
    print(circles)