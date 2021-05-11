# Dinosaur above a vent of fire OpenGL project

## How does the user interact with this executable?

To run this program, navigate to the CW2 executable folder, and execute "Project_Template.exe". After a little while, the program will run and should look like the below gif.
(PLEASE NOTE: THE PARTICLE SYSTEM WILL APPEAR AND DISAPPEAR EVERY FIVE SECONDS.)

![Working GIF](https://i.gyazo.com/cbb5b0124246e4c5a2764e9d21ba7f45.gif)

When this program is running, every five seconds, the particle system will either turn on or off - disappearing immediately. If it turns on, it may appear that it has come from nowhere - the particles are still running in the background, but the render pass is not executed.


To close out of the program, close the program normally.

## How does the program code work? How do the classes and functions fit together?

The main bulk of the executable takes place in both InitScene(), update() and render().

Within initscene(), first the shaders are compiled into their relevant GLSLPrograms, and linked. With the particle system, feedback buffers are setup with unique identifiers after the shaders are compiled, but before a link has occurred. These feedback buffers are used to store the Position of the particles, the velocity of the particles and the age of the particles.
After the compile process has taken place, assuming it all goes smoothly, the first program executed is for the SIlhouette shading. In this section, only the uniforms are set, defining the line colour, light position and intensity onto the objects, and finally the material diffuse and ambience.
Particles are next initialised - first loading and storing both the fire texture, and a generated 1D array of texture particles, 3 times the number of particles passed into the constructor.
The buffers are also initialised in the InitBuffers() function, which creates vertex arrays based on the 1D arrays of texture particles - for the position, age and velocity. This is repeated for both slots in each of the arrays.
Finally, the transform feedback's are created - with 2 created.
Each of these store 1 set of the position, age and velocity arrays - the first takes slot 0, the second takes slot 1 of each array respectively.


After that is complete, the particle GLSL program is initialised, and then relevant uniforms pased in and set.
Finally, the noise is setup - which makes up the background of the program, with the cloud effect.
Two buffers are created - with the number of vertices passed in. Afterwards, vertex arrays are generated for the quad object - the plane in the background which will host the texture, and then the handle arrays have pointers assigned, and enabled.
Finally, the noise texture for the skybox program (which has been repurposed to host the noise shaders) is set through a uniform using the same variable texture (slot 2), and then the generated texture is bound to the same slot.

In the render() loop, the quad is first rendered as it is the "background" of the program.
Next up the camera position and view matrices are set accordingly, based on the cameraPos vec3 variable.
The first model matrix to be setup is for the vent below the dinosaur. 
Due to an issue with the particle system, the camera has been rotated in the X axis, meaning that all models have to be rotated. For the vent, it has been rotated by 270 degrees, as well as translated and scaled to 10% of it's original size, as it is originally massive.
The next model setup is for the dinosaur itself. This dinosaur is also scaled to 10%, however it has two rotations applied. The first rotation in the Z axis means that it is the right way up to the camera, and then it is rotated 180 degrees in the X axis to flip it around and have the head facing the camera.

Finally, the model matrix is setup for the particle system. Here is where the startTimer variable is initialised, as this is an easier way of tracking the time - since this won't be reached until the model has loaded in, there will not be an initial extremely short particle effect before reverting to 5 seconds each time.
The first pass for particles is completed - binding the transform feedback as well as the vertex array, and drawing the array.
If a special bool, known as EnableParticles is true, then the render pass will execute, drawing the vertex array again but as triangles, appearing on the screen, as well as updating the time.
If the variable is false, this render sequence is skipped.

Finally, in the update() loop, timers are updated. Two timers, known as "time" and "timers" only start counting once the first render loop has been completed. time is used for controlling particle ages, whereas "timer" is used to control the enableParticles.
If the timer reaches 5 seconds, it is reset and the enableparticles variable is flipped.

## Where was the idea drawn from, and what makes your shader program special?
Initially, the plan was to reuse the original dinosaur setup from the first section of the coursework, and simply change the shaders passed in. 
The first shader created was for shadows, and trying to sync this with the night vision noise did not work at all - it simply wouldn't render the shadows or the noise. Therefore I then changed to using shadows with particle systems - and the particle systems wouldn't render. I decided to scrap shadows, and try and get particle systems and noise working together - however these wouldn't work either.
So I finally decided to change to using Silhouette shading mixed with particle systems, which ran successfully. However, the particle system spawns all particles at the same time, making it harder to simulate fire. It also always moved to the left, instead of upwards. After a lot of trial and error, I determined the best fix would be to instead of rotating the emission direction, which wasn't working, rotate the camera to simulate the fire "rising". The next choice was to choose something suitable for the fire to spawn from. I quickly created a vent in Blender, which I then imported and is now used at the bottom of my project, also having silhouette shading applied, giving it a slightly brown, used colour.
Finally, the scene needed a background - the cloud noise texture was a good fit, but needed a background change. Therefore I took the existing cloud noise functionality, and replaced the background colour to be red, to simulate a darker environment, as blue clouds wouldn't work anywhere near as well, with the end product being a raptor over a firey vent.

My shader program is special because it uses a mix of the shading techniques - silhouette for the dinosaur, noise for the background and a custom particle system to simulate a burst of fire every so often. Combining these shaders to work together was a challenge which led to issues, as not all of the buffers and variables work very well together.

## Known issues:

 - Some machines may get a "spam" in the command line about the texture object 2 bound to unit 1 not having a defined base level. This does not affect overall execution of the program, although in tests I have noticed that the particle system looks slightly different.
 - The particle system is supposed to turn on and off every 5 seconds - this can be slightly finnicky with timings and may depend on your machine.
