import numpy as np
import matplotlib as mpl
mpl.use('pdf')
import matplotlib.pyplot as plt

def running_mean(x, N):
    cumsum = np.cumsum(np.insert(x, 0, 0)) 
    return (cumsum[N:] - cumsum[:-N]) / float(N)

dat=np.loadtxt('./data/eos_data.dat')

t=dat[:,0]
mom=dat[:,1:4]

T=dat[:,4]

#print(t.shape, mom.shape, T.shape)

#plt.figure()
#plt.plot(t*0.01, mom[:,0], label='x')
#plt.plot(t*0.01, mom[:,1], label='y')
#plt.plot(t*0.01, mom[:,2], label='z')
#
#plt.legend()
#plt.xlabel(r'$\tau$')
#plt.ylabel(r'$p$')
#
#plt.savefig('./plots/momentum.pdf')

########## Temperature #########
tMean = running_mean(t, 50)
TMean = running_mean(T, 50)

plt.figure()
plt.plot(t*0.01, T, 'r.')
plt.plot(tMean*0.01, TMean, 'k')

plt.xlabel(r'$\tau$')
plt.ylabel(r'$T$')

plt.savefig('./plots/temperature.pdf')
