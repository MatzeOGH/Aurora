# Aurora Project

Aurora is a C++ application that serves as an example project using CMake for build management.

## Project Structure

```
aurora
├── src
│   ├── main.cpp
│   └── CMakeLists.txt
├── build
├── CMakeLists.txt
└── README.md
```

## Building the Project

To build the project, follow these steps:

1. Navigate to the `build` directory:
   ```
   cd build
   ```

2. Run CMake to configure the project:
   ```
   cmake ..
   ```

3. Build the project using:
   ```
   cmake --build .
   ```

## Running the Application

After building the project, you can run the application with the following command:

```
./aurora
```

## License

This project is licensed under the MIT License. See the LICENSE file for more details.