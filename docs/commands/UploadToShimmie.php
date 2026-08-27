<?php

/* Settings */

    /* Shimmie Settings */
        // The URL of Shimmie's login form.
        $loginURL = 'localhost/shimmie/user_admin/login';

        // The URL of Shimmie's built-in upload form.
        $uploadURL = 'localhost/shimmie/upload';

        $shimmieUser        = 'admin';
        $shimmiePassword    = '';

        // True if the ratings extension is enabled.
        $enableRating = true;

        // Any extra tags that should be applied to the image.
        // Should be a space-delimited list.
        $extraTags = 'technical:grabber';

    /* Database Settings */
        $servername = 'localhost';
        $username   = 'root';
        $password   = '';
        $dbname     = 'shimmie';
        $port       = null; // null for localhost. Usually 3306, otherwise.

    /* Argument Settings */
        // Argument list.
        $opts  = "";
        $opts .= "h:"; // Hash.
        $opts .= "f:"; // File with path.
        $opts .= "t::"; // Tags.
        $opts .= "s::"; // Source.
        $opts .= "r::"; // Rating.

        // Argument defaults, if the real values
        // are blank or missing.
        $defaults =
        [
            't' => 'tagme',
            's' => 'Grabber',
            'r' => 'u'
        ];

        // The delimiter used by Grabber.
        // Set to ' ' to skip delimiter replacement.
        $tagDelimiter = ';';

    /* Upload Settings */
        // Mapping to replace categories given by Grabber with
        // whatever we'd rather they be.
        $categoryReplacements =
        [
            //'general:' => '',
            //'unknown:' => '',
        ];

        // Mapping to replace individual tags.
        // Should take the form of category:tag => newCategory:newTag
        // where the original category is after remapping with $categoryReplacements.
        // The category on the replacement tag is optional.
        $tagReplacements =
        [
            //'category:tag' => 'newCategory:newTag',
            //'category:tag2' => 'newTag2'
        ];

        // Import tag replacement files.
        // Each file should look like:
        //  <?php
        //      $tagReplacements +=
        //      [
        //          'category:tag' => 'newCategory:newTag'
        //      ];
        if(file_exists("tagReplacements"))
        {
            foreach (glob("tagReplacements/*.php") as $filename)
            {
                include $filename;
            }
        }

    /* Misc Settings */
        // True to delete the image from where Grabber saved it
        // after being uploaded.
        $deleteOrig = false;



/* Process Arguments */
    // Retrieve our arguments.
    $options = getopt($opts);

    // Handle the arguments.
    $hash = $options['h'];
    $file = $options['f'];
    $tags = GetOptOrDefault($options, 't', $defaults);
    $source = GetOptOrDefault($options, 's', $defaults);
    $rating = GetOptOrDefault($options, 'r', $defaults);

    /* Tag Processing */
        // Shimmie wants tags as space-delimited. Replace
        // whatever delimiter Grabber gave us with spaces.
        $tags = str_replace($tagDelimiter, ' ', $tags);
        $tags = htmlentities($tags, ENT_NOQUOTES | ENT_SUBSTITUTE, "UTF-8");

        // Replace the categories given to us with what is
        // specified in the settings.
        foreach($categoryReplacements as $oldCategory => $newCategory)
        {
            $tags = str_replace($oldCategory, $newCategory, $tags);
        }

        // Replace tags with what is specified in the settings.
        foreach($tagReplacements as $tag => $newTag)
        {
            $pattern = '/(^|\s)(' . preg_quote($tag) . ')($|\s)/i';
            $tags = preg_replace($pattern, '$1'.$newTag.'$3', $tags);
        }

    /* Rating Processing */
        // Shimmie accepts ['u', 's', 'q', 'e'].
        $rating = strtolower(substr($rating, 0, 1));



/* Login To Shimmie */
    // Set post fields.
    $post =
    [
        'user' => $shimmieUser,
        'pass' => $shimmiePassword
    ];

    // POST the form.
    $ch = curl_init($loginURL);
    curl_setopt($ch, CURLOPT_POST,1);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $post);
    curl_setopt($ch, CURLOPT_COOKIESESSION, true);
    curl_setopt($ch, CURLOPT_COOKIEJAR, 'cookiejar.txt');
    curl_setopt($ch, CURLOPT_COOKIEFILE, 'cookiefile.txt');

    $response = curl_exec($ch);

/* Image Upload */
    // Retrieve the image.
    if (function_exists('curl_file_create'))
    {
        $cFile = curl_file_create($file);
    }
    else
    {
        $cFile = '@' . realpath($file);
    }

    // Set post fields.
    $post =
    [
        'data0' => $cFile,
        'tags' => $tags . ' ' . $extraTags,
        'source' => $source
    ];

    // POST the form.
    curl_setopt($ch, CURLOPT_URL, $uploadURL);
    curl_setopt($ch, CURLOPT_POST, 1);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $post);
    curl_setopt($ch, CURLOPT_COOKIESESSION, true);
    curl_setopt($ch, CURLOPT_COOKIEJAR, 'cookiejar.txt');
    curl_setopt($ch, CURLOPT_COOKIEFILE, 'cookiefile.txt');

    $response = curl_exec($ch);

    // Dump cURL errors.
    if ($response === false)
    {
        echo 'cURL error! Aborting.';
        var_dump(curl_getinfo($ch));
        var_dump(curl_errno($ch));
        var_dump(curl_error($ch));

        die();
    }

    // Make sure the upload succeeded.
    if (strpos($response, 'You should be redirected to') === false)
    {
        if (strpos($response, 'already has hash') !== false)
        {
            echo 'Image with that hash already existed.';

            // If requested to delete the source image...
            if($deleteOrig)
            {
                // Delete the file from the staging area.
                unlink($file);
            }
        }
        else
        {
            echo 'Something went wrong! Aborting.';
            var_dump($response);

            die();
        }
    }

    curl_close($ch);



/* Post Processing */
    // If the ratings extension is enabled...
    if ($enableRating)
    {
        // Connect to the database.
        $conn = new PDO("mysql:host=$servername;dbname=$dbname;" . (isset($port) ? "port=$port" : ''), $username, $password);

        // Set the rating.
        $query = $conn->prepare("UPDATE images SET rating = '$rating' WHERE hash = '$hash'");
        $query->execute();
    }



/* Cleanup */
    // If requested to delete the source image...
    if($deleteOrig)
    {
        // Delete the file from the staging area.
        unlink($file);
    }



// Returns an argument's value. If missing, returns the specified default.
function GetOptOrDefault($argumentList, $key, $defaults)
{
    return array_key_exists($key, $argumentList) ? $argumentList[$key] : $defaults[$key];
}