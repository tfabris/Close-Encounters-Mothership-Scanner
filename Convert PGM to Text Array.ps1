# ---------------------------------------------------------------------------
# Utility to convert a grayscale PGM file into a monochrome pixel array.
# ---------------------------------------------------------------------------
# 
# This Powershell Core script is to help create some of the 1-bit pixel arrays
# that I'll be using for the Arduino code for an LED lighting project.
#
# The reason I'm using PGM files is so that it's easy to parse, since they are
# simple ASCII. It's easy to create a PGM in most image editors (I'm using GIMP
# but you can use others): Create the image as a grayscale or a black and white
# image. Draw your image, draw it as close to pure "black and white" as you
# can. Some parts of the image may be anti-aliased, i.e, shades of gray, as you
# draw your image. Increase the image contrast to the maximum possible
# contrast, to ensure that you are seeing only black pixels or white pixels,
# which gets rid of any anti-aliasing. Edit the image as necessary to
# compensate for the contrast change. Once you have the image looking correct
# and only containing black pixels or white pixels, then, export the file to a
# PGM file. Then feed the PGM file to this program and it will output a TXT
# file containing the one-bit array that can be pasted into Arduino code.
#
# PGM format:    https://users.wpi.edu/~cfurlong/me-593n/pgmimage.html
#
# ---------------------------------------------------------------------------

# Ensure that any file, path, or parsing errors will stop the program.
$ErrorActionPreference = "Stop"

# Get the path and filename of the PGM file from the user.
$filename = Read-Host "Enter the path and filename of the PGM file (including the extension)"

# Trim a possible trailing space off the file name. This allows you to drag the
# file onto the terminal window on Mac systems (as long as it doesn't have any
# spaces in the path).
$filename = $filename.trim()

# Prepare the output file name and other name-related variables.
$outputFilename = [System.IO.Path]::ChangeExtension($filename, "txt")
$nameOnly = [System.IO.Path]::GetFileNameWithoutExtension($filename)
$noSpecialChars = $nameOnly -Replace '[^0-9A-Z]', ' '
$camelCaseName = (Get-Culture).TextInfo.ToTitleCase($noSpecialChars) -Replace ' '
$camelCaseArrayName = "array" + $camelCaseName

# Read the contents of the PGM file.
$content = Get-Content -Raw $filename

# Convert into an array of lines, stripping out any comment lines.
$lines = $content -split '\r?\n' | Where-Object { $_ -notmatch '^\s*#' } 

# Parse the rows and columns, which is two numbers separated by a space on the
# second line. We have already stripped out the comment on the original second
# line, so this is the "new" second line after stripping out the comment.
$cols = [int]($lines[1] -split ' ')[0]
$rows = [int]($lines[1] -split ' ')[1]

# Print the number of rows and columns that were just parsed.
Write-Host "`"$camelCaseArrayName`" image size: $($cols) x $($rows)"

# Prepare variables to gather the final output.
$output = ""
$pixelCountPosition = 0

# Begin the array definition string
$output += "bool $($camelCaseArrayName)[] = `r`n"
$output += "{ `r`n"

# PGM files can have multiple lines, and each pixel in the file is an ASCII
# number which can be separated by either spaces or line breaks. The ones from
# my GIMP output had every pixel on its own line separated by CRLFs. The first
# line of pixel data begins at the fourth line in the file, which is line
# index [3] after we already stripped out the comment on the second line.
foreach ($line in $lines[3..($lines.Length-1)])
{
    $values = $line -split '\s+'
    foreach ($value in $values)
    {
        # Ensure that the value is an actual number as opposed to just blank
        # space or a carriage return. Some PGM files may have a blank line at
        # the end which would add an extra zero at the end of the array if we
        # don't do this step.
        if ([string]::IsNullOrEmpty($value))
        {
            # Debugging output in case we have trouble parsing a file.
            Write-Host "Source file contained one value that was null or empty at pixel $pixelCountPosition"
        }
        else
        {
            # Convert the grayscale values to binary values based on brightness, and
            # output the resulting array as comma values like 1,0,0,1,0,0,1, etc.,
            # with 0 representing any dark pixel and 1 representing any white pixel.
            $binaryValue = if ($value -ge 128) { "1" } else { "0" }
            $output += "$binaryValue,"

            # Keep track of which pixel we're on, so that we can add a carriage
            # return for each row of the output array. This makes it easier to see
            # the image laid out in the resulting pixel array.
            $pixelCountPosition ++

            # Use the Modulo operator to flag where each row ends and insert a
            # carriage return there.
            if ($pixelCountPosition % $cols -eq 0)
            {
                $output += "`r`n"
            }
        }
    }
}

# Finish the array definition string
$output += "};`r`n"
$output += "int $($camelCaseArrayName)Size = sizeof($($camelCaseArrayName));`r`n"
$output += "int $($camelCaseArrayName)Width = $cols;`r`n"

# Write the output file, overwriting one if it's already there.
$output | Out-File $outputFilename -Encoding ASCII
Write-Host "Converted PGM file saved to $($outputFilename)"

