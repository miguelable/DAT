<?php

function log_error($message)
{
    echo "\033[31m[ERROR] $message\033[0m"; // Rojo
}

function log_warning($message)
{
    echo "\033[33m[WARNING] $message\033[0m"; // Amarillo
}

function log_info($message)
{
    echo "\033[32m[INFO] $message\033[0m"; // Verde
}

function log_debug($message)
{
    echo "\033[34m[DEBUG] $message\033[0m"; // Azul
}

function log_verbose($message)
{
    echo "\033[37m[VERBOSE] $message\033[0m"; // Blanco
}