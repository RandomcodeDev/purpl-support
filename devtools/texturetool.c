/*++

Copyright (c) 2024 MobSlicer152

Module Name:

    texturetool.c

Abstract:

    This file implements a rudimentary tool for working with textures.

--*/

#include "purpl/purpl.h"

#include "common/alloc.h"
#include "common/common.h"

#include "util/texture.h"

_Noreturn VOID Usage(VOID)
/*++

Routine Description:

    Prints instructions for using the program and exits.

Arguments:

    None.

Return Value:

    Does not return.

--*/
{
    LogInfo("Usage:");
    LogInfo("\tto <input image> <output Purpl texture>\t\t\t- Create a texture");
    LogInfo("\tfrom <input Purpl texture> <output PNG image>\t\t- Convert a ");
    LogInfo("texture to a regular image");
    exit(EINVAL);
}

UINT32
ConvertFormat(UINT32 Format, BOOLEAN ToStb)
/*++

Routine Description:

    Converts between stb and Purpl formats.

Arguments:

    Format - The input format.

    ToStb - Specifies whether the input is an stb or Purpl format.

Return Value:

    An equivalent format or a default.

--*/
{
    if (ToStb)
    {
        switch (Format)
        {
        default:
        case TextureFormatUndefined:
            return STBI_default;
        case TextureFormatDepth:
            return STBI_grey;
        case TextureFormatRgba8:
            return STBI_rgb_alpha;
        case TextureFormatRgb8:
            return STBI_rgb;
        }
    }
    else
    {
        switch (Format)
        {
        default:
        case STBI_default:
            return TextureFormatUndefined;
        case STBI_grey:
            return TextureFormatDepth;
        case STBI_rgb_alpha:
            return TextureFormatRgba8;
        case STBI_rgb:
            return TextureFormatRgb8;
        }
    }
}

INT ConvertTo(_In_ PCSTR Source, _In_ PCSTR Destination)
/*++

Routine Description:

    Converts images to Purpl textures.

Arguments:

    Source - The image to convert.

    Destination - The Purpl texture to write to.

Return Value:

    0 on success or an appropriate errno code.

--*/
{
    PBYTE ImageBytes;
    UINT32 Width;
    UINT32 Height;
    UINT32 Format;
    PTEXTURE Texture;

    LogInfo("Converting image %s to Purpl texture %s", Source, Destination);

    LogInfo("Loading image %s", Source);
    ImageBytes =
        stbi_load(Source, (PINT32)&Width, (PINT32)&Height, (PINT32)&Format,
                  STBI_rgb_alpha // Vulkan doesn't like R8G8B8_SRGB
        );
    if (!ImageBytes)
    {
        LogError("Failed to load image %s: %s", Source, strerror(errno));
        return errno;
    }

    LogInfo("Creating Purpl texture");
    Texture = CreateTexture(TextureFormatRgba8,
                            //        ConvertFormat(
                            //            Format,
                            //            FALSE
                            //            ),
                            Width, Height, ImageBytes);
    if (!Texture)
    {
        LogError("Failed to convert image %s", Source);
        return errno;
    }

    LogInfo("Writing texture to %s", Destination);
    if (!WriteTexture(Destination, Texture))
    {
        LogError("Failed to write texture to %s: %s", Destination,
                strerror(errno));
        return errno;
    }

    stbi_image_free(ImageBytes);
    CmnFree(Texture);
    LogInfo("Done");
    return 0;
}

INT ConvertFrom(_In_ PCSTR Source, _In_ PCSTR Destination)
/*++

Routine Description:

    Converts Purpl textures to PNG.

Arguments:

    Source - The Purpl texture to convert.

    Destination - The image to write to.

Return Value:

    0 on success or an appropriate errno code.

--*/
{
    // PBYTE ImageBytes;
    PTEXTURE Texture;

    LogInfo("Converting Purpl texture %s to PNG %s", Source, Destination);

    LogInfo("Loading texture %s", Source);
    Texture = LoadTexture(Source);
    if (!Texture)
    {
        LogError("Failed to load texture %s: %s", Source,
                strerror(errno));
        return errno;
    }

    LogInfo("Writing PNG %s from format %d texture", Destination,
           Texture->Format);
    stbi_write_png(Destination, Texture->Width, Texture->Height,
                   (INT32)GetFormatComponents(Texture->Format), Texture->Pixels,
                   (INT32)Texture->Width *
                       (INT32)GetFormatPitch(Texture->Format));

    CmnFree(Texture);
    LogInfo("Done");
    return 0;
}

//
// Tool mode
//

typedef enum TEXTURETOOL_MODE
{
    TextureToolModeNone,
    TextureToolModeConvertTo,
    TextureToolModeConvertFrom,
    TextureToolModeCount
} TEXTURETOOL_MODE, *PTEXTURETOOL_MODE;

//
// Functions for each mode
//

typedef INT (*PFNTEXTURETOOL_OPERATION)(_In_ PCSTR Source, _In_ PCSTR Destination);
PFNTEXTURETOOL_OPERATION Operations[TextureToolModeCount] = {
    (PFNTEXTURETOOL_OPERATION)Usage,
    ConvertTo,
    ConvertFrom,
};

INT main(INT argc, PCHAR *argv)
/*++

Routine Description:

    Processes arguments and calls the requested function.

Arguments:

    argc - Number of arguments.

    argv - Array of arguments.

Return Value:

    EXIT_SUCCESS - Success.

    errno value - Failure.

--*/
{
    TEXTURETOOL_MODE Mode;
    INT Result;

    LogInfo("Purpl Texture Tool v" PURPL_VERSION_STRING
           " (supports texture format v" PURPL_STRINGIZE_EXPAND(
               TEXTURE_FORMAT_VERSION) ") on %s",
           PlatGetDescription());

    CmnInitialize(NULL, 0);

    if (argc < 4)
    {
        Mode = TextureToolModeNone;
    }
    else if (strcmp(argv[1], "to") == 0)
    {
        Mode = TextureToolModeConvertTo;
    }
    else if (strcmp(argv[1], "from") == 0)
    {
        Mode = TextureToolModeConvertFrom;
    }
    else
    {
        Mode = TextureToolModeNone;
    }

    Result = Operations[Mode](argv[2], argv[3]);

    CmnShutdown();

    return Result;
}
