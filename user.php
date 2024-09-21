<?php

$ip = "127.0.0.10";
$port = 8888;

$server_ip = "127.0.0.1";
$server_port = 8888;

// get download directory
$download_directory = "users/" . basename(__DIR__) . "/download/";
$shared_directory = "users/" . basename(__DIR__) . "/shared/";

echo $download_directory;
echo $shared_directory;