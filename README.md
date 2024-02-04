In this project, a network has been created using *ns3* and *Docker*. In the setup folder, you can find the necessary installations for the proper functioning of the ns3-Network.cc file.

<p align = "center">
  The image below represents the network created
  <img src = "https://github.com/LucaTamSapienza/ns3Project/assets/155265433/11a58814-1583-4d09-96eb-ff6dded48c99" width = 700, heigth = 300/>
</p>


To observe the correct operation of the latter, follow the instructions below:

- Ensure that ns3 is installed correctly by referring to the instructions in the **setup** folder.
- Build ns3 according to the provided setup instructions.
- Place the file ns3-Network.cc inside the scratch folder.

These steps will allow you to observe the final result of the project.
>[!tip]
>For a correct use of Docker, see the instructions below:


To see all the image downloaded run:
```sh
docker ps -a
```
To see all the running container
```sh
docker ps
```
To run the container with the image downloaded during the installation
```sh
docker start ns-3-corso
```
To stop the running container
```sh
docker stop ns-3-corso
```


|Version|Changes|
|:-------:|:-------:|
|ReduceL2-1|initial version|
|ReduceL2-11|`axes` range is now [-r, -r-1], r = rank(data)|
|ReduceL2-13|added `T`:  tensor(bfloat16)|
|ReduceL2-18|`axes` attributes is now `noop_with_empty_axes` : int (default is 0) <br> `data` (differentiable) : T)|

