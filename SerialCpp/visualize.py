### Visualization of KMeans final output
import pandas as pd
import matplotlib.pyplot as plt

# Read df.
df = pd.read_csv('output.txt') 

# Create plot and show.
ax = df.plot.scatter(x='x', y='y',c='cluster', colormap='viridis')
x = df['x']
y = df['y']
c = df['cluster']

plt.scatter(x,y,c)
plt.show()