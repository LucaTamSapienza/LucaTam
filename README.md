In this project, a network has been created using *ns3* and *Docker*. In the setup folder, you can find the necessary installations for the proper functioning of the ns3-Network.cc file.

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
