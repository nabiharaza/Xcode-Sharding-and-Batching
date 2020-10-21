//
// Copyright SpiRITlab - The Panther project.
// https://github.com/SpiRITlab/Panther
//

#include "XCode.h"
#include "util.h"
#include "Cache.h"
#include <string>
#include <vector>

/*
 * XCodeTableBase
 */

template<class T>
Panther::XCodeTableBase<T>::XCodeTableBase(int cols, int rows) : _columns(cols), _rows(rows) {
}

template<class T>
T
Panther::XCodeTableBase<T>::get(int colID, int rowID) {
    return xcodeTable[colID][rowID];
}

template<class T>
void
Panther::XCodeTableBase<T>::assign(int colID, int rowID, T val) {
    xcodeTable[colID][rowID] = val;
}

template<class T>
void
Panther::XCodeTableBase<T>::print() {
    for (int c = 0; c < _columns; ++c) {
        for (int r = 0; r < _rows; ++r) {
            std::cout << xcodeTable[r][c] << "  ";
        }
        std::cout << std::endl;
    }
}

template<class T>
void
Panther::XCodeTableBase<T>::swtichLastTwoRows() {
    T tmp;
    for (int c = 0; c < _columns; ++c) {
        tmp = xcodeTable[c][_rows - 2];
        xcodeTable[c][_rows - 2] = xcodeTable[c][_rows - 1];
        xcodeTable[c][_rows - 1] = tmp;
    }
}

/***/
template<class T>
Panther::XCodeLabelTable<T>::XCodeLabelTable(int cols, int rows) : XCodeTableBase<T>(cols, rows) {
    for (int c = 0; c < cols; ++c) {
        std::vector<T> column;
        for (int r = 0; r < rows; ++r) {
            column.push_back(std::to_string(r) + ',' + std::to_string(c));
        }
        this->xcodeTable.push_back(column);
    }
}

/*
 *
 */
template<class T>
Panther::XCodeDataTable<T>::XCodeDataTable(int cols, int rows) : XCodeTableBase<T>(cols, rows) {
    for (int c = 0; c < cols; ++c) {
        std::vector<T> column(rows);
        this->xcodeTable.push_back(column);
    }
}


/*
 * XCode
 */

template<class T>
Panther::XCode<T>::XCode(int n) : n(n) {
    this->part_1_labelTable = new Panther::XCodeLabelTable<std::string>(n, n);
    this->part_2_labelTable = new Panther::XCodeLabelTable<std::string>(n, n);
}

template<class T>
Panther::XCodeLabelTable<std::string> *
Panther::XCode<T>::getPart1LabelTable() {
    return part_1_labelTable;
}

template<class T>
Panther::XCodeLabelTable<std::string> *
Panther::XCode<T>::getPart2LabelTable() {
    return part_2_labelTable;
}

template<class T>
void
Panther::XCode<T>::printPart1LabelTable() {
    part_1_labelTable->print();
}

template<class T>
void
Panther::XCode<T>::printPart2LabelTable() {
    part_2_labelTable->print();
}

template<class T>
void
Panther::XCode<T>::encode(std::vector<Panther::Shard<T>> &shards, Panther::Dataset<T> &dataset) {
    int cols = n, rows = n;

    Panther::XCodeDataTable<bool> xCodeDataTable(n, n);
    for (int c = 0; c < cols; ++c) {
        for (int r = 0; r < rows - 2; ++r) {
            xCodeDataTable.assign(c, r, dataset.get(r, c));
        }
    }
    util::debug("Filling xCodeDataTable...");
    xCodeDataTable.print();

    // calculate parity bits
    bool part1Tmp, part2Tmp;
    int part1RowID = rows - 2;
    int part2RowID = rows - 1;
    for (int c = 0; c < cols; ++c) {
//        std::cout << "part 1: " << mod(part1RowID-1,rows) << " " << mod(c+1,cols)  << std::endl;
        part1Tmp = xCodeDataTable.get(mod(c + 1, cols), mod(part1RowID - 1, rows));
//        std::cout << "part 2: " << mod(part2RowID-2,rows) << " " << mod(c-1,cols) << std::endl;
        part2Tmp = xCodeDataTable.get(mod(c - 1, cols), mod(part2RowID - 2, rows));

        for (int r = 2; r < rows - 1; ++r) {
//            std::cout << "part 1: " << (part1RowID-r)%rows << " " << (c+r)%cols  << std::endl;
            part1Tmp ^= xCodeDataTable.get(mod(c + r, cols), mod(part1RowID - r, rows));
//            std::cout << "part 2: " << mod(part2RowID-r-1,rows) << " " << mod(c-r,cols)  << std::endl;
            part2Tmp ^= xCodeDataTable.get(mod(c - r, cols), mod(part2RowID - r - 1, rows));
        }
        xCodeDataTable.assign(c, part1RowID, part1Tmp);
        xCodeDataTable.assign(c, part2RowID, part2Tmp);
    }

    util::debug("Calculated parity bits.");
    xCodeDataTable.print();

    int rotationCnt = 0;
    Panther::XCodeLabelTable<std::string> originalLabelMap(*part_1_labelTable);
    util::debug("Part 1: ");
    for (int c = 0; c < cols; ++c) {
        Panther::Shard<T> shard;
        rotationCnt = (cols - 1) - c;
        for (int r = 0; r < rows; ++r) {
            shard.addToPart1(xCodeDataTable.get(c, (r + rotationCnt) % rows));
            part_1_labelTable->assign(c, r, originalLabelMap.get(c, (r + rotationCnt) % rows));
        }
        shards.push_back(shard);
        std::cout << "shard" << c << ": ";
        shard.printPart1();
    }

    util::debug("Part 2: ");
    xCodeDataTable.swtichLastTwoRows();
    originalLabelMap.swtichLastTwoRows();
    for (int c = 0; c < cols; ++c) {
        rotationCnt = c;
        for (int r = 0; r < rows; ++r) {
            shards[c].addToPart2(xCodeDataTable.get(c, (r + rotationCnt) % rows));
            part_2_labelTable->assign(c, r, originalLabelMap.get(c, (r + rotationCnt) % rows));
        }
        std::cout << "shard" << c << ": ";
        shards[c].printPart2();
    }
}


template<class T>
void
Panther::XCode<T>::recoverFromOneColumnErasure(Panther::Encryptor &encryptor, std::vector<Panther::Shard<T>> &shards,
                                               int colID) {
    int rows = n, cols = n;
    helib::Ctxt recoveredShard_part1(*(encryptor.getPublicKey()));
    helib::Ctxt recoveredShard_part2(*(encryptor.getPublicKey()));
    int colIdx;
    std::string label;
    Panther::Cache cache;

    // part 1
    util::debug("Recovering part 1 with one column erasure...\n\n");
    for (int r = 0; r < rows; ++r) {
        if (r != colID) { // skip one computation for the parity bit
            helib::Ctxt ctxt(*(encryptor.getPublicKey()));
            helib::Ptxt<helib::BGV> bitmask(*(encryptor.getContext()));
            for (int c = 1; c < cols; ++c) {
                std::cout << "----- For  row and Coloumn:  " << r << "," << c << "  ------" << std::endl << std::endl;
                colIdx = mod(colID + c, cols);
                std::cout << "ColIdx Before Check if colIdx !=r : " << r << "," << colIdx << std::endl;
                if (colIdx != r) {
                    std::cout << "ColIdx After" << r << "," << colIdx << std::endl;
                    std::cout << "row: " << r << " colIdx: " << colIdx << std::endl;
//                   encryptor.decryptAndPrint("ctxt_before", ctxt);
//                   encryptor.decryptAndPrint("shardctxt", *(shards[colIdx].getPart1Ctxt()));
                    ctxt.addCtxt(*(shards[colIdx].getPart1Ctxt()));
//                   encryptor.decryptAndPrint("ctxt_after", ctxt);
                }
            }
            bitmask[r] = 1;
            std::cout << "bitmask: " << bitmask << std::endl;
            ctxt.multByConstant(bitmask);
//            encryptor.decryptAndPrint("ctxt_after_bitmask", ctxt);
            recoveredShard_part1.addCtxt(ctxt);
//            encryptor.decryptAndPrint("recoveredShard_part1_adding", recoveredShard_part1);
            std::cout << "In Rotated Matrix: " << r << "," << colID << std::endl;
            label = this->part_1_labelTable->get(colID, r);
            std::cout << "Actual Matrix (label): " << label << std::endl;
            cache.add(label, new Panther::CacheEntry(r, colID, new helib::Ctxt(ctxt)));
        }
    }
    cache.print();
    // part 2
    util::debug("Recovering part 2...");
    for (int r = 0; r < rows; ++r) {
        if (r != rows - colID - 1) { // skip one computation for the parity bit
            label = this->part_2_labelTable->get(colID, r);
//            std::cout << "label: " << label << std::endl;
            if (!cache.contain(label)) {
                util::debug("computed value doesn't exist, compute it now.");
                helib::Ctxt ctxt(*(encryptor.getPublicKey()));
                helib::Ptxt<helib::BGV> bitmask(*(encryptor.getContext()));
                for (int c = 1; c < cols; ++c) {
                    colIdx = mod(colID + c, cols);
                    if (colIdx != rows - r - 1) {
                        std::cout << "row: " << r << " colIdx: " << colIdx << std::endl;
                        ctxt.addCtxt(*(shards[colIdx].getPart2Ctxt()));
                    }
                }
                bitmask[r] = 1;
                ctxt.multByConstant(bitmask);
                recoveredShard_part2.addCtxt(ctxt);

                label = this->part_2_labelTable->get(colID, r);
                cache.add(label, new Panther::CacheEntry(r, colID, new helib::Ctxt(ctxt)));
            } else { // if found, this means it has been computed in part 1
                util::debug("reusing computed value.");
//               encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
                std::cout << "rotation " << r - cache.get(label)->getRowID() << std::endl;
//                encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()), r - cache.get(label)->getRowID());
//               encryptor.decryptAndPrint("after rotation", *(cache.get(label)->getCtxt()));
                recoveredShard_part2.addCtxt(*(cache.get(label)->getCtxt()));
            }
        }
    }

    cache.print();

    util::debug("populating missing values...");
    label = this->part_1_labelTable->get(colID, colID);
    std::cout << "label: " << label << std::endl;
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    std::cout << "rotation " << colID - cache.get(label)->getRowID() << std::endl;
    encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()), colID - cache.get(label)->getRowID());
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    recoveredShard_part1.addCtxt(*(cache.get(label)->getCtxt()));

    label = this->part_2_labelTable->get(colID, rows - colID - 1);
    std::cout << "label: " << label << std::endl;
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    std::cout << "rotation " << rows - colID - 1 - cache.get(label)->getRowID() << std::endl;
    encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()),
                                          rows - colID - 1 - cache.get(label)->getRowID());
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    recoveredShard_part2.addCtxt(*(cache.get(label)->getCtxt()));

    encryptor.decryptAndPrint("RecoveredShardPart1", recoveredShard_part1);
    encryptor.decryptAndPrint("RecoveredShardPart2", recoveredShard_part2);
}

template<class T>
void
Panther::XCode<T>::recoverFromTwoColumnErasure(Panther::Encryptor &encryptor, std::vector<Panther::Shard<T>> &shards,
                                               int colID1, int colID2) {
    int rows = n, cols = n;
    helib::Ctxt recoveredShard_part1(*(encryptor.getPublicKey()));
    helib::Ctxt recoveredShard_part2(*(encryptor.getPublicKey()));
    std::string label;
    std::string label1;
    Panther::Cache cache;
    std::vector<std::string> failedElements;
    int diagonalCount = 0;
    int failedPerRow = 0;
    int inCache = 0;
    int colIdx1 = 0;
    int colIdx2 = 0;
    std::string brokenElementD1 = "";
    std::string brokenElementD2 = "";
    bool workingRowFlag = false;

    for (int r = 0; r < rows; ++r) {
        workingRowFlag = false;
        label = this->part_1_labelTable->get(colID1, r);
        label1 = this->part_1_labelTable->get(colID2, r);
        if (!cache.contain(label) || !cache.contain(label1)) { //check if it is not in the cache
            helib::Ctxt ctxt(*(encryptor.getPublicKey()));
            helib::Ptxt<helib::BGV> bitmask(*(encryptor.getContext()));
            for (int c = 1; c < cols; ++c) { // now check each col for the same row
                {
                    std::cout << "\n\n-----Part 1: For  row and column: " << r << "," << c << "------" << std::endl;
                    if (r == colID1 || r == colID2) // checking if one of the faulty one is parity
                    {
                        colIdx1 = mod(colID1 + c, cols);
//                        std::cout << "colIdx1: " << colIdx1 << std::endl;
                        if (r != colIdx1 && colIdx1 != colID1 &&
                            colIdx1 != colID2) {     // skipping the parity and adding the good ones in ctxt
//                            std::cout << "-----Starting Inserting for " << r << "," << c << "------" << std::endl;
                            std::cout << "Inserted: " << r << "," << colIdx1 << "" << std::endl << std::endl;
//                            encryptor.decryptAndPrint("ctxt_before", ctxt);
//                            encryptor.decryptAndPrint("shardctxt", *(shards[colIdx1].getPart1Ctxt()));
                            workingRowFlag = true;
                            ctxt.addCtxt(*(shards[colIdx1].getPart1Ctxt()));
//                            encryptor.decryptAndPrint("ctxt_after", ctxt);
                        }
                    }
                }
                //adding all the good ones are done. Now apply bitmask and XOR!
                //workingRowFlag so that masks are not added to other non working rows
                if (workingRowFlag == true) {
                    bitmask[r] = 1;
                    std::cout << "bitmask: " << bitmask << std::endl;
                    ctxt.multByConstant(bitmask);
//                          encryptor.decryptAndPrint("ctxt_after_bitmask", ctxt);
                    recoveredShard_part1.addCtxt(ctxt);
//                            encryptor.decryptAndPrint("recoveredShard_part1_adding", recoveredShard_part1);
                    if (r != colID1) {
                        std::cout << "Recovering Part 1 Disk 1 " << std::endl;
                        std::cout << "In Rotated Matrix: " << r << "," << colID1 << std::endl;
                        label = this->part_1_labelTable->get(colID1, r);
                        std::cout << "Actual Matrix (label): " << label << std::endl;
                        cache.add(label, new Panther::CacheEntry(r, colID1, new helib::Ctxt(ctxt)));
                        std::cout << "Label:  " << label << " is successfully added to cache and recovered."
                                  << std::endl;
                    }
                    if (r != colID2) {
                        std::cout << "Recovering Part 1 Disk 2 " << std::endl;
                        std::cout << "In Rotated Matrix: " << r << "," << colID2 << std::endl;
                        label = this->part_1_labelTable->get(colID2, r);
                        std::cout << "Actual Matrix (label): " << label << std::endl;
                        cache.add(label, new Panther::CacheEntry(r, colID2, new helib::Ctxt(ctxt)));
                        std::cout << "Label:  " << label << " is successfully added to cache and recovered."
                                  << std::endl;
                    }

                }


            }
            cache.print();

        } else {
            //Caching goes here
        }


    }

    //part 2
    std::cout << "\n\n Recovering Part 2.....\n" << std::endl;
    for (int r = 0; r < rows; ++r) {
        workingRowFlag = false;
        label = this->part_1_labelTable->get(colID1, r);
        label1 = this->part_1_labelTable->get(colID2, r);
        if (!cache.contain(label) || !cache.contain(label1)) { //check if it is not in the cache
            helib::Ctxt ctxt(*(encryptor.getPublicKey()));
            helib::Ptxt<helib::BGV> bitmask(*(encryptor.getContext()));
            for (int c = 1; c < cols; ++c) { // now check each col for the same row
                {
                    std::cout << "\n\n-----Part 2: For  row and column: " << r << "," << c << "------" << std::endl;
                    if (r == colID1 || r == colID2) // checking if one of the faulty one is parity
                    {
                        colIdx1 = mod(colID1 + c, cols);
                        std::cout << "colIdx1: " << colIdx1 << std::endl;
                        if (colIdx1 != rows - r - 1 && colIdx1 != colID1 &&
                            colIdx1 != colID2) {     // skipping the parity and adding the good ones in ctxt
                            std::cout << "-----Starting Inserting for " << r << "," << c << "------" << std::endl;
                            std::cout << "Inserted: " << r << "," << colIdx1 << "" << std::endl << std::endl;
//                            encryptor.decryptAndPrint("ctxt_before", ctxt);
//                            encryptor.decryptAndPrint("shardctxt", *(shards[colIdx1].getPart1Ctxt()));
                            workingRowFlag = true;
                            ctxt.addCtxt(*(shards[colIdx1].getPart1Ctxt()));
//                            encryptor.decryptAndPrint("ctxt_after", ctxt);
                        }
                    }
                }
            }
            //adding all the good ones are done. Now apply bitmask and XOR!
            //workingRowFlag so that masks are not added to other non working rows
            if (workingRowFlag == true) {
                bitmask[r] = 1;
                std::cout << "bitmask: " << bitmask << std::endl;
                ctxt.multByConstant(bitmask);
//              encryptor.decryptAndPrint("ctxt_after_bitmask", ctxt);
                recoveredShard_part2.addCtxt(ctxt);
//              encryptor.decryptAndPrint("recoveredShard_part1_adding", recoveredShard_part1);
                if (r != rows - 1 - colID1) {
                    std::cout << "Recovering Part 2 Disk 1 " << std::endl;
                    std::cout << "In Rotated Matrix: " << r << "," << colID1 << std::endl;
                    label = this->part_2_labelTable->get(colID1, r);
                    std::cout << "Actual Matrix (label): " << label << std::endl;
                    cache.add(label, new Panther::CacheEntry(r, colID1, new helib::Ctxt(ctxt)));
                    std::cout << "Label:  " << label << " is successfully added to cache and recovered."
                              << std::endl;
                }
                if (r != rows - 1 - colID2) {
                    std::cout << "Recovering Part 2 Disk 2 " << std::endl;
                    std::cout << "In Rotated Matrix: " << r << "," << colID2 << std::endl;
                    label = this->part_2_labelTable->get(colID2, r);
                    std::cout << "Actual Matrix (label): " << label << std::endl;
                    cache.add(label, new Panther::CacheEntry(r, colID2, new helib::Ctxt(ctxt)));
                    std::cout << "Label:  " << label << " is successfully added to cache and recovered."
                              << std::endl;
                }


            }
        }
    }
    cache.print();
}


template
class Panther::XCode<bool>;