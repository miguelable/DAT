<?php

// autoload_static.php @generated by Composer

namespace Composer\Autoload;

class ComposerStaticInitb1c373ffd6fef5a820c75c6d64bf7c67
{
    public static $prefixLengthsPsr4 = array (
        'B' => 
        array (
            'Bluerhinos\\' => 11,
        ),
    );

    public static $prefixDirsPsr4 = array (
        'Bluerhinos\\' => 
        array (
            0 => __DIR__ . '/..' . '/bluerhinos/phpmqtt',
        ),
    );

    public static $classMap = array (
        'Composer\\InstalledVersions' => __DIR__ . '/..' . '/composer/InstalledVersions.php',
    );

    public static function getInitializer(ClassLoader $loader)
    {
        return \Closure::bind(function () use ($loader) {
            $loader->prefixLengthsPsr4 = ComposerStaticInitb1c373ffd6fef5a820c75c6d64bf7c67::$prefixLengthsPsr4;
            $loader->prefixDirsPsr4 = ComposerStaticInitb1c373ffd6fef5a820c75c6d64bf7c67::$prefixDirsPsr4;
            $loader->classMap = ComposerStaticInitb1c373ffd6fef5a820c75c6d64bf7c67::$classMap;

        }, null, ClassLoader::class);
    }
}
