void write(struct file_metadata *metadata, enum fs_retcode *return_code) {
	struct node_filesystem node_fs_buffer;
	struct sector_filesystem sector_fs_buffer;
	struct map_filesystem map_fs_buffer;
	// Add the required data types

	// Load the filesystem from storage to memory

	// 1. Look for nodes with the same parent name and location on the node.
	//    If no match is found, do the 2nd process.
	// 	  If a match is found, write the retcode
	// 	  FS_W_FILE_ALREADY_EXIST and exit.

	// 2. Search for empty entries on the node's filesystem and save the index.
	//    If there is an empty entry, save the index for writing.
	//    If there is no empty entry, write FS_W_MAXIMUM_NODE_ENTRY
	//    and exit.

	// 3. Check and make sure the entry node at index P is a folder.
	//    The root is defined as a folder.
	//    If the index is a file or an empty entry,
	//    Write the retcode FS_W_INVALID_FOLDER and exit.

	// 4. With the filesize metadata information, calculate the sectors
	//    that are still empty in the filesystem map. Each byte of the map
	//    represents one sector so each byte represents 512 bytes of storage. If
	//    empty space is not satisfied, write the retcode FS_W_NOT_ENOUGH_STORAGE
	//    and exit. If the filesize exceeds 8192 bytes, write the retcode
	//    FS_W_NOT_ENOUGH_STORAGE and exit.
	//    If empty space is available, continue with step 5.

	// 5. Check the filesystem sector for empty entries.
	//    If there is an empty entry and you want to write the file, save the index
	//    for the write. If there is no empty entry and you are going to write the
	//    file, write FS_W_MAXIMUM_SECTOR_ENTRY and exit. Other than the above
	//    conditions, continue to the writing process.

	// Writing
	// Write the name metadata and P bytes to the node in buffer memory
	// If writing a folder, write an S byte with the value of
	// FS_NODE_S_IDX_FOLDER and skip to step 8
	// 3. If writing a file, also write the S byte corresponding to the sector
	// index
	// 4. Prepare variable j = 0 for the empty sector entry iterator
	// 5. Prepare a buffer variable for an empty entry sector
	// 6. Perform the following iterations with looping conditions
	// (unfinished writing && i = 0..255)
	// 1. Check whether map[i] is filled or not
	// 2. If filled, continue to the next iteration / continue
	// 3. Mark map[i] as filled
	// Replace the jth byte of the sector entry buffer with i
	// 5. Increase the value of j by 1
	// 6. Perform writeSector() with the file pointer buffer in the metadata
	// and destination sector i
	// 7. If the size of the file that has been written is greater than or equal
	// to the the filesize in the metadata, the write is complete

	// 7. Update with memcpy() the sector entry buffer with the
	// the filesystem sector buffer
	// 8. Write the entire filesystem (map, node, sector) to storage
	// using writeSector() on the appropriate sector
	// 9. Return the FS_SUCCESS retcode
}