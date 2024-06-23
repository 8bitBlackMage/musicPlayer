//
// Created by Alice Exley on 22/06/2024.
//

#ifndef MUSIC_PLAYER_LIBRARY_H
#define MUSIC_PLAYER_LIBRARY_H

/**
 * @brief Object that manages a library of music files, storing track metadata.
 * @details This Class uses TagLib to read track metadata. all data is stored in an unsorted Value Tree.
 * Getter functions can optionally sort by track number or release year.
 * each Entry is given a Unique ID
 *
 * Library
 *      -> Artist[ Artist Name ]
 *              -> Album [Album Name, Album Release Year]
 *                      -> Track [Track Number, Track Name, FilePath]
 *
 */
class Library
{
public:
    static const juce::Identifier idLibrary;
    static const juce::Identifier idArtistName;
    static const juce::Identifier idAlbumName;
    static const juce::Identifier idTrackTitle;
    static const juce::Identifier idTrackDuration;
    static const juce::Identifier idTrackFilePath;
    static const juce::Identifier idTrackGenre;
    static const juce::Identifier idAlbumYear;
    static const juce::Identifier idTrackNumber;

    Library();

    /**
     * @brief Scans a folder for music files and adds them to the list of songs in library.
     * @param rootFolder root folder to scan.
     */
    void scanFolder(juce::File & rootFolder);

    /**
     * @brief Saves current Value Tree state to compressed binary format using GZip.
     * @param outputFile destination file to be saved.
     * @return true if successful, false if not.
     */
    bool writeToBinaryFile(juce::File &outputFile);
    /**
     * @brief Reads saved state into library value tree. decompresses using Gzip.
     * @param inputFile
     * @return
     */
    bool readFromBinaryFile(juce::File &inputFile);

    /**
     * @brief Search Library for an album by the name of the album and artist.
     * @details NOTE: this is an exact search only, fuzzy searching is not yet an option.
     * @param artistName Name of the artist to search.
     * @param albumName Name of the album to search.
     * @return Value tree, containing album info if found.
     */
    juce::ValueTree getAlbumDataFromNameAndArtist(const juce::String &artistName,const juce::String &albumName);

    /**
     * @brief Search Library for an album using only the name of the album.
     * @details NOTE: this is an exact search only, fuzzy searching is not yet an option.
     * @param albumName Name of the album to search.
     * @return Value tree, containing album info if found.
     */
    juce::ValueTree getAlbumDataFromName(const juce::String &albumName);

    /**
     * @brief Search Library for Artist and retrun all data for that artist.
     * @details NOTE: this is an exact search only, fuzzy searching is not yet an option.
     * @param artistName Name of artist to search for
     * @return Value Tree, containing artist info if found.
     */
    juce::ValueTree getArtistDataFromName(const juce::String &artistName);


private:

    /**
     * @brief Sort Value tree children by integer property, such as Year of Release or Track number.
     * @param tree Tree to sort
     * @param sortKey key to sort by.
     */
    void sortValueTree(juce::ValueTree& tree, const juce::Identifier& sortKey);

    /**
     * @brief Types of Tag within the Library database
     */
    enum IdentifierType
    {
        album,
        artist,
        track
    };
    /**
     * @brief Generates a XML Safe identifier for a given bit of data that wants to be stored in the library
     * @param value Value to encode.
     * @param identifierType type of data to encode.
     * @return string containing encoded identifier.
     */
    juce::String getXMLIdentifier(const juce::String &value, IdentifierType identifierType );

    juce::AudioFormatManager manager;
    juce::Array<juce::File> searchPaths;
    juce::ValueTree libraryTree;

};


#endif //MUSIC_PLAYER_LIBRARY_H
