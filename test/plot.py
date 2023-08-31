import plotly.graph_objects as go
from plotly.subplots import make_subplots
with open('log.txt', 'r') as file:
    lines = file.readlines()
Cmdms = []
m1Cmd = []
m2Cmd = []
m3Cmd = []
s1Cmd = []
s2Cmd = []
duration = []

Anglems = []
m1 = []
m2 = []
m3 = []
s1 = []
s2 = []
curDuration1 = []

Velms = []
velM1 = []
velM2 = []
velM3 = []
velS1 = []
velS2 = []
curDuration2 = []



for line in lines:
    # 如果行以 * 开头，则解析数据
    if line.startswith('Cmdms'):
        # 分割每行内容
        CmdmsTokens = line.strip().split(',')
        for token in CmdmsTokens:
            key, value = token.strip().split(':')
            if key == "Cmdms":
                Cmdms.append(float(value))
            if key == "m1Cmd":
                m1Cmd.append(float(value))
            if key == "m2Cmd":
                m2Cmd.append(float(value))
            if key == "m3Cmd":
                m3Cmd.append(float(value))
            if key == "s1Cmd":
                s1Cmd.append(float(value))
            if key == "s2Cmd":
                s2Cmd.append(float(value))
            if key == "duration":
                duration.append(float(value))
    if line.startswith('Anglems'):
        # 分割每行内容
        AnglemsTokens = line.strip().split(',')
        for token in AnglemsTokens:
            key, value = token.strip().split(':')
            if key == "Anglems":
                Anglems.append(float(value))
            if key == "m1":
                m1.append(float(value))
            if key == "m2":
                m2.append(float(value))
            if key == "m3":
                m3.append(float(value))
            if key == "s1":
                s1.append(float(value))
            if key == "s2":
                s2.append(float(value))
            if key == "duration":
                curDuration1.append(float(value))               
    if line.startswith('Velms'):
        # 分割每行内容
        VelmsTokens = line.strip().split(',')
        for token in VelmsTokens:
            key, value = token.strip().split(':')
            if key == "Velms":
                Velms.append(float(value))
            if key == "velM1":
                velM1.append(float(value))
            if key == "velM2":
                velM2.append(float(value))
            if key == "velM3":
                velM3.append(float(value))
            if key == "velS1":
                velS1.append(float(value))
            if key == "velS2":
                velS2.append(float(value))
            if key == "duration":
                curDuration2.append(float(value))
fig = make_subplots(rows=1, cols=2)


# 添加第一个折线图轨迹
fig.add_trace(go.Scatter(x=duration, y=m1Cmd, mode='lines+markers', name='m1Cmd'), row=1, col=1)

# 添加第二个折线图轨迹
fig.add_trace(go.Scatter(x=duration, y=m2Cmd, mode='lines+markers', name='m2Cmd'), row=1, col=1)

fig.add_trace(go.Scatter(x=duration, y=m3Cmd, mode='lines+markers', name='m3Cmd'), row=1, col=1)

fig.add_trace(go.Scatter(x=duration, y=s1Cmd, mode='lines+markers', name='s1Cmd'), row=1, col=1)

fig.add_trace(go.Scatter(x=duration, y=s2Cmd, mode='lines+markers', name='s1Cmd'), row=1, col=1)


# 添加第一个折线图轨迹
fig.add_trace(go.Scatter(x=curDuration1, y=m1, mode='lines+markers', name='curM1'), row=1, col=2)

# 添加第二个折线图轨迹
fig.add_trace(go.Scatter(x=curDuration1, y=m2, mode='lines+markers', name='curM2'), row=1, col=2)

fig.add_trace(go.Scatter(x=curDuration1, y=m3, mode='lines+markers', name='curM3'), row=1, col=2)

fig.add_trace(go.Scatter(x=curDuration1, y=s1, mode='lines+markers', name='curS1'), row=1, col=2)

fig.add_trace(go.Scatter(x=curDuration1, y=s2, mode='lines+markers', name='curS2'), row=1, col=2)



# 设置图表布局
fig.update_layout(title='Multiple Lines on One Chart',
                  xaxis_title='ms',
                  yaxis_title='angle&vel')

# 显示图表
fig.show()



      






