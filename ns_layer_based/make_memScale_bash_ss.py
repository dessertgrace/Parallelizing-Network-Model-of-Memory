import os

nts = [4, 8, 16, 20, 24, 36, 64, 128, 256]

os.makedirs('./out/', exist_ok=True)

for nt in nts:
    with open(f'ss_{nt}.sh', 'w') as f:
        f.write(f'#!/bin/bash\n')
        f.write(f'#SBATCH --job-name=ss_{str(nt).zfill(2)}\n')
        f.write(f'#SBATCH --output=out/ss_{str(nt).zfill(2)}.out\n')
        f.write(f'#SBATCH --nodes={nt//68 + 1}\n')
        f.write(f'#SBATCH --ntasks={nt}\n')
        f.write(f'#SBATCH --cpus-per-task=1\n')
        f.write(f'#SBATCH --time=2:00:00\n')
        f.write(f'#SBATCH -p normal\n')
        f.write(f'#SBATCH -A TG-MDE210001\n\n')

        f.write(f'./multi_ns_parallel -p b_01 -t FATAL --caseID=ss_{str(nt).zfill(2)} -f -n {nt} 1')

with open('runMemScaleSS.sh', 'w') as f:
    f.write(f'#!/bin/bash\n\n')
    f.write(f'for i in {" ".join((str(i) for i in (nts)))}\n')
    f.write('do\n')
    f.write('    sbatch ss_$i.sh\n')
    f.write('done')
    
os.system('chmod +x runMemScaleSS.sh')