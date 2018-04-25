#include "utilities/crypto.h"

#include <boost/algorithm/hex.hpp>
#include <boost/scoped_array.hpp>
#include <iostream>

#include <sodium.h>

#include "logging.h"
#include "openssl_compat.h"
#include "utilities/utils.h"

std::string PublicKey::TypeString() const {
  switch (type) {
    case RSA:
      return "rsa";
    case ED25519:
      return "ed25519";
    default:
      return "UNKNOWN";
  }
}

std::string Crypto::sha256digest(const std::string &text) {
  unsigned char sha256_hash[crypto_hash_sha256_BYTES];
  crypto_hash_sha256(sha256_hash, reinterpret_cast<const unsigned char *>(text.c_str()), text.size());
  return std::string(reinterpret_cast<char *>(sha256_hash), crypto_hash_sha256_BYTES);
}

std::string Crypto::sha512digest(const std::string &text) {
  unsigned char sha512_hash[crypto_hash_sha512_BYTES];
  crypto_hash_sha512(sha512_hash, reinterpret_cast<const unsigned char *>(text.c_str()), text.size());
  return std::string(reinterpret_cast<char *>(sha512_hash), crypto_hash_sha512_BYTES);
}

std::string Crypto::RSAPSSSign(ENGINE *engine, const std::string &private_key, const std::string &message) {
  StructGuard<EVP_PKEY> key(nullptr, EVP_PKEY_free);
  StructGuard<RSA> rsa(nullptr, RSA_free);
  if (engine != nullptr) {
    key.reset(ENGINE_load_private_key(engine, private_key.c_str(), nullptr, nullptr));

    if (key == nullptr) {
      LOG_ERROR << "ENGINE_load_private_key failed with error " << ERR_error_string(ERR_get_error(), nullptr);
      return std::string();
    }

    rsa.reset(EVP_PKEY_get1_RSA(key.get()));
    if (rsa == nullptr) {
      LOG_ERROR << "EVP_PKEY_get1_RSA failed with error " << ERR_error_string(ERR_get_error(), nullptr);
      return std::string();
    }
  } else {
    StructGuard<BIO> bio(BIO_new_mem_buf(const_cast<char *>(private_key.c_str()), static_cast<int>(private_key.size())),
                         BIO_vfree);
    key.reset(PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr, nullptr));
    if (key != nullptr) {
      rsa.reset(EVP_PKEY_get1_RSA(key.get()));
    }

    if (rsa == nullptr) {
      LOG_ERROR << "PEM_read_bio_PrivateKey failed with error " << ERR_error_string(ERR_get_error(), nullptr);
      return std::string();
    }

#if AKTUALIZR_OPENSSL_PRE_11
    RSA_set_method(rsa.get(), RSA_PKCS1_SSLeay());
#else
    RSA_set_method(rsa.get(), RSA_PKCS1_OpenSSL());
#endif
  }

  const unsigned int sign_size = RSA_size(rsa.get());
  boost::scoped_array<unsigned char> EM(new unsigned char[sign_size]);
  boost::scoped_array<unsigned char> pSignature(new unsigned char[sign_size]);

  std::string digest = Crypto::sha256digest(message);
  int status = RSA_padding_add_PKCS1_PSS(rsa.get(), EM.get(), reinterpret_cast<const unsigned char *>(digest.c_str()),
                                         EVP_sha256(), -1 /* maximum salt length*/);
  if (status == 0) {
    LOG_ERROR << "RSA_padding_add_PKCS1_PSS failed with error " << ERR_error_string(ERR_get_error(), nullptr);
    return std::string();
  }

  /* perform digital signature */
  status = RSA_private_encrypt(RSA_size(rsa.get()), EM.get(), pSignature.get(), rsa.get(), RSA_NO_PADDING);
  if (status == -1) {
    LOG_ERROR << "RSA_private_encrypt failed with error " << ERR_error_string(ERR_get_error(), nullptr);
    return std::string();
  }
  std::string retval = std::string(reinterpret_cast<char *>(pSignature.get()), sign_size);
  return retval;
}

std::string Crypto::Sign(KeyType key_type, ENGINE *engine, const std::string &private_key, const std::string &message) {
  if (key_type == kED25519) {
    return Crypto::ED25519Sign(boost::algorithm::unhex(private_key), message);
  }
  return Crypto::RSAPSSSign(engine, private_key, message);
}

std::string Crypto::ED25519Sign(const std::string &private_key, const std::string &message) {
  unsigned char sig[crypto_sign_BYTES];
  crypto_sign_detached(sig, nullptr, reinterpret_cast<const unsigned char *>(message.c_str()), message.size(),
                       reinterpret_cast<const unsigned char *>(private_key.c_str()));
  return std::string(reinterpret_cast<char *>(sig), crypto_sign_BYTES);
}

std::string Crypto::getKeyId(const std::string &key) {
  std::string key_content = key;

  boost::algorithm::trim_right_if(key_content, boost::algorithm::is_any_of("\n"));
  std::string keyid = boost::algorithm::hex(Crypto::sha256digest(Json::FastWriter().write(Json::Value(key_content))));
  std::transform(keyid.begin(), keyid.end(), keyid.begin(), ::tolower);
  return keyid;
}

bool Crypto::RSAPSSVerify(const std::string &public_key, const std::string &signature, const std::string &message) {
  StructGuard<RSA> rsa(nullptr, RSA_free);
  StructGuard<BIO> bio(BIO_new_mem_buf(const_cast<char *>(public_key.c_str()), static_cast<int>(public_key.size())),
                       BIO_vfree);
  {
    RSA *r = nullptr;
    if (PEM_read_bio_RSA_PUBKEY(bio.get(), &r, nullptr, nullptr) == nullptr) {
      LOG_ERROR << "PEM_read_bio_RSA_PUBKEY failed with error " << ERR_error_string(ERR_get_error(), nullptr);
      return false;
    }
    rsa.reset(r);
  }

#if AKTUALIZR_OPENSSL_PRE_11
  RSA_set_method(rsa.get(), RSA_PKCS1_SSLeay());
#else
  RSA_set_method(rsa.get(), RSA_PKCS1_OpenSSL());
#endif

  const unsigned int size = RSA_size(rsa.get());
  boost::scoped_array<unsigned char> pDecrypted(new unsigned char[size]);
  /* now we will verify the signature
    Start by a RAW decrypt of the signature
  */
  int status =
      RSA_public_decrypt(static_cast<int>(signature.size()), reinterpret_cast<const unsigned char *>(signature.c_str()),
                         pDecrypted.get(), rsa.get(), RSA_NO_PADDING);
  if (status == -1) {
    LOG_ERROR << "RSA_public_decrypt failed with error " << ERR_error_string(ERR_get_error(), nullptr);
    return false;
  }

  std::string digest = Crypto::sha256digest(message);

  /* verify the data */
  status = RSA_verify_PKCS1_PSS(rsa.get(), reinterpret_cast<const unsigned char *>(digest.c_str()), EVP_sha256(),
                                pDecrypted.get(), -2 /* salt length recovered from signature*/);

  return status == 1;
}
bool Crypto::ED25519Verify(const std::string &public_key, const std::string &signature, const std::string &message) {
  return crypto_sign_verify_detached(reinterpret_cast<const unsigned char *>(signature.c_str()),
                                     reinterpret_cast<const unsigned char *>(message.c_str()), message.size(),
                                     reinterpret_cast<const unsigned char *>(public_key.c_str())) == 0;
}

bool Crypto::VerifySignature(const PublicKey &public_key, const std::string &signature, const std::string &message) {
  if (public_key.type == PublicKey::ED25519) {
    return ED25519Verify(boost::algorithm::unhex(public_key.value), Utils::fromBase64(signature), message);
  }
  if (public_key.type == PublicKey::RSA) {
    return RSAPSSVerify(public_key.value, Utils::fromBase64(signature), message);
  }
  LOG_ERROR << "unsupported keytype: " << public_key.type;
  return false;
}

bool Crypto::parseP12(BIO *p12_bio, const std::string &p12_password, std::string *out_pkey, std::string *out_cert,
                      std::string *out_ca) {
#if AKTUALIZR_OPENSSL_PRE_11
  SSLeay_add_all_algorithms();
#endif
  StructGuard<PKCS12> p12(d2i_PKCS12_bio(p12_bio, nullptr), PKCS12_free);
  if (p12 == nullptr) {
    LOG_ERROR << "Could not read from " << p12_bio << " file pointer";
    return false;
  }

  // use a lambda here because sk_X509_pop_free is a macro
  auto stackx509_free = [](STACK_OF(X509) * stack) {
    sk_X509_pop_free(stack, X509_free);  // NOLINT
  };

  StructGuard<EVP_PKEY> pkey(nullptr, EVP_PKEY_free);
  StructGuard<X509> x509_cert(nullptr, X509_free);
  StructGuard<STACK_OF(X509)> ca_certs(nullptr, stackx509_free);
  {
    EVP_PKEY *pk;
    X509 *x509c = nullptr;
    STACK_OF(X509) *cacs = nullptr;
    if (PKCS12_parse(p12.get(), p12_password.c_str(), &pk, &x509c, &cacs) == 0) {
      LOG_ERROR << "Could not parse file from " << p12_bio << " source pointer";
      return false;
    }
    pkey.reset(pk);
    x509_cert.reset(x509c);
    ca_certs.reset(cacs);
  }

  StructGuard<BIO> pkey_pem_sink(BIO_new(BIO_s_mem()), BIO_vfree);
  if (pkey_pem_sink == nullptr) {
    LOG_ERROR << "Could not open pkey buffer for writing";
    return false;
  }
  PEM_write_bio_PrivateKey(pkey_pem_sink.get(), pkey.get(), nullptr, nullptr, 0, nullptr, nullptr);

  char *pkey_buf;
  auto pkey_len = BIO_get_mem_data(pkey_pem_sink.get(), &pkey_buf);  // NOLINT
  *out_pkey = std::string(pkey_buf, pkey_len);

  char *cert_buf;
  size_t cert_len;
  StructGuard<BIO> cert_sink(BIO_new(BIO_s_mem()), BIO_vfree);
  if (cert_sink == nullptr) {
    LOG_ERROR << "Could not open certificate buffer for writing";
    return false;
  }
  PEM_write_bio_X509(cert_sink.get(), x509_cert.get());

  char *ca_buf;
  size_t ca_len;
  StructGuard<BIO> ca_sink(BIO_new(BIO_s_mem()), BIO_vfree);
  if (ca_sink == nullptr) {
    LOG_ERROR << "Could not open ca buffer for writing";
    return false;
  }
  X509 *ca_cert = nullptr;
  for (int i = 0; i < sk_X509_num(ca_certs.get()); i++) {  // NOLINT
    ca_cert = sk_X509_value(ca_certs.get(), i);            // NOLINT
    PEM_write_bio_X509(ca_sink.get(), ca_cert);
    PEM_write_bio_X509(cert_sink.get(), ca_cert);
  }
  ca_len = BIO_get_mem_data(ca_sink.get(), &ca_buf);  // NOLINT
  *out_ca = std::string(ca_buf, ca_len);

  cert_len = BIO_get_mem_data(cert_sink.get(), &cert_buf);  // NOLINT
  *out_cert = std::string(cert_buf, cert_len);

  return true;
}

bool Crypto::extractSubjectCN(const std::string &cert, std::string *cn) {
  StructGuard<BIO> bio(BIO_new_mem_buf(const_cast<char *>(cert.c_str()), static_cast<int>(cert.size())), BIO_vfree);
  StructGuard<X509> x(PEM_read_bio_X509(bio.get(), nullptr, nullptr, nullptr), X509_free);
  if (x == nullptr) {
    return false;
  }

  int len = X509_NAME_get_text_by_NID(X509_get_subject_name(x.get()), NID_commonName, nullptr, 0);
  if (len < 0) {
    return false;
  }
  boost::scoped_array<char> buf(new char[len + 1]);
  X509_NAME_get_text_by_NID(X509_get_subject_name(x.get()), NID_commonName, buf.get(), len + 1);
  *cn = std::string(buf.get());
  return true;
}

/**
 * Generate a RSA keypair
 * @param public_key Generated public part of key
 * @param private_key Generated private part of key
 * @return true if the keys are present at the end of this function (either they were created or existed already)
 *         false if key generation failed
 */
bool Crypto::generateRSAKeyPair(KeyType key_type, std::string *public_key, std::string *private_key) {
  int bits = (key_type == kRSA2048) ? 2048 : 4096;
  int ret = 0;

#if AKTUALIZR_OPENSSL_PRE_11
  StructGuard<RSA> rsa(RSA_generate_key(bits,    /* number of bits for the key - 2048 is a sensible value */
                                        RSA_F4,  /* exponent - RSA_F4 is defined as 0x10001L */
                                        nullptr, /* callback - can be NULL if we aren't displaying progress */
                                        nullptr  /* callback argument - not needed in this case */
                                        ),
                       RSA_free);
#else
  StructGuard<BIGNUM> bne(BN_new(), BN_free);
  ret = BN_set_word(bne.get(), RSA_F4);
  if (ret != 1) {
    return false;
  }
  StructGuard<RSA> rsa(RSA_new(), RSA_free);
  ret = RSA_generate_key_ex(rsa.get(), bits, /* number of bits for the key - 2048 is a sensible value */
                            bne.get(),       /* exponent - RSA_F4 is defined as 0x10001L */
                            nullptr          /* callback argument - not needed in this case */
                            );
  if (ret != 1) {
    return false;
  }
#endif

  StructGuard<EVP_PKEY> pkey(EVP_PKEY_new(), EVP_PKEY_free);
  // release the rsa pointer here, pkey is the new owner
  EVP_PKEY_assign_RSA(pkey.get(), rsa.release());  // NOLINT
  char *pubkey_buf;
  StructGuard<BIO> pubkey_sink(BIO_new(BIO_s_mem()), BIO_vfree);
  if (pubkey_sink == nullptr) {
    return false;
  }
  ret = PEM_write_bio_PUBKEY(pubkey_sink.get(), pkey.get());
  if (ret != 1) {
    return false;
  }
  auto pubkey_len = BIO_get_mem_data(pubkey_sink.get(), &pubkey_buf);  // NOLINT
  *public_key = std::string(pubkey_buf, pubkey_len);

  char *privkey_buf;
  StructGuard<BIO> privkey_sink(BIO_new(BIO_s_mem()), BIO_vfree);
  if (privkey_sink == nullptr) {
    return false;
  }

  ret = PEM_write_bio_RSAPrivateKey(privkey_sink.get(), static_cast<RSA *>(EVP_PKEY_get0(pkey.get())), nullptr, nullptr,
                                    0, nullptr, nullptr);
  if (ret != 1) {
    return false;
  }
  auto privkey_len = BIO_get_mem_data(privkey_sink.get(), &privkey_buf);  // NOLINT
  *private_key = std::string(privkey_buf, privkey_len);
  return true;
}

bool Crypto::generateEDKeyPair(std::string *public_key, std::string *private_key) {
  unsigned char pk[crypto_sign_PUBLICKEYBYTES];
  unsigned char sk[crypto_sign_SECRETKEYBYTES];
  crypto_sign_keypair(pk, sk);
  *public_key = boost::algorithm::hex(std::string(reinterpret_cast<char *>(pk), crypto_sign_PUBLICKEYBYTES));
  // std::transform(public_key->begin(), public_key->end(), public_key->begin(), ::tolower);
  *private_key = boost::algorithm::hex(std::string(reinterpret_cast<char *>(sk), crypto_sign_SECRETKEYBYTES));
  // std::transform(private_key->begin(), private_key->end(), private_key->begin(), ::tolower);
  return true;
}

bool Crypto::generateKeyPair(KeyType key_type, std::string *public_key, std::string *private_key) {
  if (key_type == kED25519) {
    return Crypto::generateEDKeyPair(public_key, private_key);
  }
  return Crypto::generateRSAKeyPair(key_type, public_key, private_key);
}