from mpi4py import MPI

comm = MPI.COMM_WORLD
size = comm.Get_size()
rank = comm.Get_rank()

if rank==1:
  data="sai"+str(rank)
if rank==2:
  data="rohith"+str(rank)
if rank==3:
  data="varshith"+str(rank)
if rank==0:
  data="areti"+str(rank)

print(data)

data=comm.gather(data,root=0)

if rank==0:
  print(data)





















# from mpi4py import MPI

# comm = MPI.COMM_WORLD
# size = comm.Get_size()
# rank = comm.Get_rank()

# if rank == 0:
#     k=["sai","rohith","areti","varshith"]
#     data = [k[i] for i in range(size)]
# else:
#     data = None
# data = comm.scatter(data, root=0)

# if rank==1:
#  print(data,"The rank is",rank)
# if rank==2:
#   print(data,"The rank is",rank)
# if rank==3:
#   print(data,"The rank is",rank)
# if rank==0:
#   print(data,"The rank is",rank)




# from mpi4py import MPI

# comm=MPI.COMM_WORLD
# rank=comm.Get_rank()
# size=comm.Get_size()
# print(rank,size)
# total=None
# if rank==0:
#     data=1
#     # for i in range(1,3):
#     #     comm.send(data,dest=i)
# else:
#     data=10
#     # data1=comm.recv(source=0)
#     # print("The rank of the process",rank)
#     # for key in data1:
#     #     print(key,'->',data1[key])
# total=comm.reduce(data,root=0)
# if rank==0:
#   print(total)

# MPI.Finalize
# print("The rank of process",rank)
# for key in data:
#      print(key,'->',data[key])

